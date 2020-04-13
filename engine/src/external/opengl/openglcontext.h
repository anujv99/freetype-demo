
/*
 *  Implementation of the graphics context class interface
 */

#ifndef __OPENGL_CONTEXT_H__
#define __OPENGL_CONTEXT_H__

#include <core/graphicscontext.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

namespace engine { namespace external { namespace opengl {

    class OpenGLContext : public engine::core::GraphicsContext {
    public:
        OpenGLContext();
        ~OpenGLContext();
    public:
        virtual void CreateContext(uintptr_t rawWindowPtr) override;
        virtual void BeginFrame() override;
        virtual void EndFrame() override;
    private:
        ::GLFWwindow * m_Window;
    };

} } }

#endif //__OPENGL_CONTEXT_H__