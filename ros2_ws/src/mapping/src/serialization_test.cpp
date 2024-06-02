#include "bonxai/bonxai.hpp"
#include "bonxai/serialization.hpp"
#include <fstream>

int main() {
  const double VOXEL_RESOLUTION = 0.3;
  const double RADIUS = 5;

  Bonxai::VoxelGrid<float> grid(VOXEL_RESOLUTION);
  auto accessor = grid.createAccessor();

  for (double x = -RADIUS; x <= RADIUS; x += VOXEL_RESOLUTION) {
    for (double y = -RADIUS; y <= RADIUS; y += VOXEL_RESOLUTION) {
      for (double z = 0; z <= RADIUS; z += VOXEL_RESOLUTION) {
        if (x * x + y * y + z * z <= RADIUS * RADIUS) {
          accessor.setValue(grid.posToCoord(x, y, z), sqrt(x * x + y * y + z * z)/RADIUS);
        }
      }
    }
  }

  std::ofstream outputFile("test.bx", std::ios::binary);
  if (!outputFile.is_open()) {
    std::cerr << "Error: Unable to open file for writing" << std::endl;
    return 1;
  }

  Bonxai::Serialize(outputFile, grid);
  outputFile.close();

  return 0;
}
