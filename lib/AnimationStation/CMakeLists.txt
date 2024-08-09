add_library(AnimationStation
src/Effects/Breath.cpp
src/Effects/ButtonBreath.cpp
src/Effects/CloseAll.cpp
src/Effects/Chase.cpp
src/Effects/CustomTheme.cpp
src/Effects/CustomThemePressed.cpp
src/Effects/Rainbow.cpp
src/Effects/StaticColor.cpp
src/Effects/StaticTheme.cpp
src/AnimationStation.cpp
src/Animation.cpp
)
target_include_directories(AnimationStation PUBLIC 
src
src/Effects
)
target_link_libraries(AnimationStation PUBLIC
pico_stdlib
pico_rand
hardware_clocks
hardware_timer
NeoPico
)
