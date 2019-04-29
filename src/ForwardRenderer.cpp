#include "ForwardRenderer.h"

#include <bigg.hpp>
#include <bx/string.h>
#include <bx/math.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp>

ForwardRenderer::ForwardRenderer(const Scene* scene) : Renderer(scene)
{

}

void ForwardRenderer::initialize()
{
    Renderer::initialize();

    // TODO do this somewhere else (probably Scene)
    PosColorVertex::init();

    char vsName[32];
    char fsName[32];

    const char* dir = shaderDir();

    bx::strCopy(vsName, BX_COUNTOF(vsName), dir);
    bx::strCat(vsName, BX_COUNTOF(vsName), "vs_cubes.bin");

    bx::strCopy(fsName, BX_COUNTOF(fsName), dir);
    bx::strCat(fsName, BX_COUNTOF(fsName), "fs_cubes.bin");

    mProgram = bigg::loadProgram(vsName, fsName);
    mVbh = bgfx::createVertexBuffer(bgfx::makeRef(scene->s_cubeVertices, sizeof(scene->s_cubeVertices)),
                                    PosColorVertex::ms_decl);
    mIbh = bgfx::createIndexBuffer(bgfx::makeRef(scene->s_cubeTriList, sizeof(scene->s_cubeTriList)));
    mTime = 0.0f;
}

void ForwardRenderer::reset(uint16_t width, uint16_t height)
{
    Renderer::reset(width, height);
}

void ForwardRenderer::render(float dt)
{
    float fovy = 73.74f;
    mTime += dt;
    glm::mat4 view =
        glm::lookAt(glm::vec3(0.0f, 0.0f, -35.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 proj = bigg::perspective(glm::radians(60.0f), float(width) / height, 0.1f, 100.0f);
    bgfx::setViewTransform(0, &view[0][0], &proj[0][0]);
    bgfx::setViewRect(0, 0, 0, width, height);
    //bgfx::touch(0);

    for(uint32_t yy = 0; yy < 11; ++yy)
    {
        for(uint32_t xx = 0; xx < 11; ++xx)
        {
            glm::mat4 mtx;
            mtx = glm::translate(mtx, glm::vec3(15.0f - float(xx) * 3.0f, -15.0f + float(yy) * 3.0f, 0.0f));
            mtx *= glm::yawPitchRoll(mTime + xx * 0.21f, mTime + yy * 0.37f, 0.0f);
            bgfx::setTransform(&mtx[0][0]);
            bgfx::setVertexBuffer(0, mVbh);
            bgfx::setIndexBuffer(mIbh);
            bgfx::setState(BGFX_STATE_DEFAULT);
            bgfx::submit(0, mProgram);
        }
    }
}

void ForwardRenderer::shutdown()
{
    Renderer::shutdown();

    // TODO clean up bgfx objects
}