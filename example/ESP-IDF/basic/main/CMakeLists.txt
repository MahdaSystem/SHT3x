idf_component_register(
  SRCS "main.c" "./main.c" "../../../../src/SHT3x.c" "../../../../port/ESP32-IDF/SHT3x_platform.c"
  INCLUDE_DIRS "../../../../src/include" "../../../../port/ESP32-IDF"
  )