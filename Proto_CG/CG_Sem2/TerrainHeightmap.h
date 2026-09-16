#pragma once

#include <filesystem>
#include <string>
#include <vector>
#include <utility>
#include <algorithm>
#include <cmath>
#include "stb_image.h"

struct TerrainHeightmap
{
    int Width = 0;
    int Height = 0;
    int SourceTileCount = 0;
    int TileGridSide = 1;
    std::vector<unsigned short> Heights;
};

inline bool LoadTerrainHeightmap(const std::filesystem::path& directory,
    TerrainHeightmap& result, std::string& error)
{
    namespace fs = std::filesystem;
    constexpr int side = 4;
    bool compactNames = false;
    for (int row = 1; row <= side; ++row)
        for (int column = 1; column <= side; ++column)
            if (fs::exists(directory / ("R" + std::to_string(row) + "C" + std::to_string(column) + ".png")))
                compactNames = true;
    auto tilePath = [&](int row, int column)
    {
        if (compactNames)
            return directory / ("R" + std::to_string(row + 1) + "C" + std::to_string(column + 1) + ".png");
        return directory / ("Heightmap_R" + std::to_string(row + 1) +
            "_C" + std::to_string(column + 1) + ".png");
    };
    int found = 0;
    for (int row = 0; row < side; ++row)
        for (int column = 0; column < side; ++column)
            if (fs::exists(tilePath(row, column))) ++found;

    const int sourceSide = found > 0 ? side : 1;
    const fs::path combinedPath = fs::exists(directory / "MapNG_Batch_Heightmap_Grid.png")
        ? directory / "MapNG_Batch_Heightmap_Grid.png" : directory / "Heightmap.png";
    TerrainHeightmap loaded;
    loaded.TileGridSide = found > 0 ? sourceSide :
        (combinedPath.filename() == "MapNG_Batch_Heightmap_Grid.png" ? side : 1);
    int tileWidth = 0, tileHeight = 0;
    for (int row = 0; row < sourceSide; ++row)
        for (int column = 0; column < sourceSide; ++column)
        {
            const std::string path = (found > 0 ? tilePath(row, column) : combinedPath).string();
            int width = 0, height = 0, channels = 0;
            if (!stbi_info(path.c_str(), &width, &height, &channels) || width < 2 || height < 2)
            {
                error = "Missing or invalid terrain heightmap: " + path;
                return false;
            }
            if (row == 0 && column == 0)
            {
                tileWidth = width; tileHeight = height;
                if (width > 8192 / sourceSide || height > 8192 / sourceSide)
                {
                    error = "Combined terrain heightmap must not exceed 8192 x 8192.";
                    return false;
                }
                loaded.Width = width * sourceSide;
                loaded.Height = height * sourceSide;
                loaded.Heights.resize(size_t(loaded.Width) * loaded.Height);
            }
            if (width != tileWidth || height != tileHeight)
            {
                error = "Terrain tiles must have identical dimensions: " + path;
                return false;
            }
            stbi_us* pixels = stbi_load_16(path.c_str(), &width, &height, &channels, 1);
            if (!pixels)
            {
                error = "Cannot decode terrain heightmap: " + path;
                return false;
            }
            for (int y = 0; y < height; ++y)
                std::copy_n(pixels + size_t(y) * width, width,
                    loaded.Heights.begin() + size_t(row * height + y) * loaded.Width + column * width);
            stbi_image_free(pixels);
            ++loaded.SourceTileCount;
        }
    result = std::move(loaded);
    error.clear();
    return true;
}

inline float FindTerrainTileOverlap(const TerrainHeightmap& map, bool vertical)
{
    const int length = vertical ? map.Height : map.Width;
    const int lines = vertical ? map.Width : map.Height;
    const int tile = length / map.TileGridSide;
    auto sample = [&](int line, float p)
    {
        const int a = int(p), b = (std::min)(a + 1, length - 1);
        const float t = p - a;
        auto at = [&](int i) { return float(map.Heights[vertical ? size_t(i) * map.Width + line : size_t(line) * map.Width + i]); };
        return at(a) * (1 - t) + at(b) * t;
    };
    auto error = [&](int seam, float overlap)
    {
        double sum = 0; int count = 0;
        for (int line = 0; line < lines; line += (std::max)(1, lines / 128))
            for (int i = 0; i < int(overlap); ++i)
            {
                sum += std::abs(sample(line, seam - overlap + i) - sample(line, float(seam + i)));
                ++count;
            }
        return sum / count;
    };
    double baseline = 0;
    for (int b = 1; b < map.TileGridSide; ++b) baseline += error(b * tile, 1);
    if (baseline < map.TileGridSide) return 0;
    double best = baseline; float overlap = 0;
    const int limit = (std::min)(32, tile / 8);
    for (int step = 32; step <= limit * 16; ++step)
    {
        const float candidate = step / 16.0f;
        double score = 0;
        for (int b = 1; b < map.TileGridSide; ++b) score += error(b * tile, candidate);
        if (score < best) { best = score; overlap = candidate; }
    }
    if (best >= baseline * 0.08) return 0;
    for (int b = 1; b < map.TileGridSide; ++b)
        if (error(b * tile, overlap) >= error(b * tile, 1) * 0.15) return 0;
    return overlap;
}

inline void ResampleTerrainTileOverlap(TerrainHeightmap& map, bool vertical, float overlap)
{
    const int length = vertical ? map.Height : map.Width;
    const int lines = vertical ? map.Width : map.Height;
    const int tile = length / map.TileGridSide;
    const float stride = tile - overlap;
    const float extent = length - (map.TileGridSide - 1) * overlap - 1;
    const auto source = map.Heights;
    for (int line = 0; line < lines; ++line)
        for (int i = 0; i < length; ++i)
        {
            const float p = float(i) / (length - 1) * extent;
            const int block = (std::min)(int(p / stride), map.TileGridSide - 1);
            const float local = p - block * stride;
            auto sample = [&](int b, float x)
            {
                x = (std::clamp)(x, 0.0f, float(tile - 1));
                const int a = int(x), c = (std::min)(a + 1, tile - 1);
                auto at = [&](int j) { const int u = b * tile + j; return float(source[vertical ? size_t(u) * map.Width + line : size_t(line) * map.Width + u]); };
                return at(a) + (at(c) - at(a)) * (x - a);
            };
            float value = sample(block, local);
            if (block > 0 && local < overlap - 1)
            {
                const float t = local / (overlap - 1);
                const float weight = t * t * (3 - 2 * t);
                value = sample(block - 1, local + stride) * (1 - weight) + value * weight;
            }
            map.Heights[vertical ? size_t(i) * map.Width + line : size_t(line) * map.Width + i] =
                static_cast<unsigned short>((std::clamp)(std::lround(value), 0L, 65535L));
        }
}

inline void StitchTerrainHeightmap(TerrainHeightmap& map)
{
    const int side = map.TileGridSide;
    if (side <= 1 || map.Width <= 0 || map.Height <= 0 || map.Width % side || map.Height % side ||
        map.Heights.size() != size_t(map.Width) * map.Height) return;
    const float overlapX = FindTerrainTileOverlap(map, false);
    const float overlapY = FindTerrainTileOverlap(map, true);
    if (overlapX > 0) ResampleTerrainTileOverlap(map, false, overlapX);
    if (overlapY > 0) ResampleTerrainTileOverlap(map, true, overlapY);
    const int band = (std::min)(8, (std::min)(map.Width, map.Height) / side / 8);
    if (band < 2) return;
    auto stitchLine = [&](size_t origin, size_t stride, int seam)
    {
        const int left = seam - band, right = seam + band - 1;
        auto sample = [&](int i) { return float(map.Heights[origin + size_t(i) * stride]); };
        const float a = sample(left), b = sample(right), span = float(right - left);
        const float delta = (b - a) / span;
        float slopeA = (a - sample(left - 2)) * 0.5f;
        float slopeB = (sample(right + 2) - b) * 0.5f;
        if (std::abs(delta) < 1.0e-6f) slopeA = slopeB = 0.0f;
        else
        {
            if (slopeA * delta < 0.0f) slopeA = 0.0f;
            if (slopeB * delta < 0.0f) slopeB = 0.0f;
            const float alpha = slopeA / delta, beta = slopeB / delta;
            const float magnitude = std::sqrt(alpha * alpha + beta * beta);
            if (magnitude > 3.0f) { slopeA *= 3.0f / magnitude; slopeB *= 3.0f / magnitude; }
        }
        for (int i = left + 1; i < right; ++i)
        {
            const float t = float(i - left) / span, t2 = t * t, t3 = t2 * t;
            const float y = (2 * t3 - 3 * t2 + 1) * a + (t3 - 2 * t2 + t) * span * slopeA +
                (-2 * t3 + 3 * t2) * b + (t3 - t2) * span * slopeB;
            map.Heights[origin + size_t(i) * stride] =
                static_cast<unsigned short>((std::clamp)(std::lround(y), 0L, 65535L));
        }
    };
    for (int boundary = 1; boundary < side; ++boundary)
        for (int y = 0; y < map.Height; ++y)
            if (overlapX == 0) stitchLine(size_t(y) * map.Width, 1, boundary * (map.Width / side));
    for (int boundary = 1; boundary < side; ++boundary)
        for (int x = 0; x < map.Width; ++x)
            if (overlapY == 0) stitchLine(x, map.Width, boundary * (map.Height / side));
}
