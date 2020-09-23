target_sources(appExec PRIVATE
    Engine.hpp
    base/Shader.hpp
    base/Texture.hpp
    debug/FrameTracker.hpp
    layout/GridLayout.hpp
    layout/GridShader.hpp
    layout/GridTile.hpp
)

add_subdirectory(nuklear)