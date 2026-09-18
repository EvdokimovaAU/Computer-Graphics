#pragma once

#include <filesystem>
#include <string>
#include <vector>

struct TerrainHeightmap
{
    int Width = 0;
    int Height = 0;
    int SourceTileCount = 0;
    int TileGridSide = 1;
    std::vector<unsigned short> Heights;
};

bool LoadTerrainHeightmap(const std::filesystem::path& directory,
    TerrainHeightmap& result, std::string& error);
float FindTerrainTileOverlap(const TerrainHeightmap& map, bool vertical);
void ResampleTerrainTileOverlap(TerrainHeightmap& map, bool vertical, float overlap);
void StitchTerrainHeightmap(TerrainHeightmap& map);
