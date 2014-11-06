#include <GL/glew.h>
#include <GL/glut.h>

// includes, cuda
#include <cuda_runtime.h>
#include <cuda_gl_interop.h>

// Utilities and timing functions
#include <helper_functions.h>    // includes cuda.h and cuda_runtime_api.h
#include <timer.h>               // timing functions

// CUDA helper functions
#include <helper_cuda.h>         // helper functions for CUDA error check
#include <helper_cuda_gl.h>      // helper functions for CUDA/GL interop

#include <vector_types.h>

// vbo variables
GLuint vbo;
struct cudaGraphicsResource *cuda_vbo_resource;
void *d_vbo_buffer = NULL;



void launch_kernel()
{
    // execute the kernel
    dim3 block(8, 8, 1);
    //dim3 grid(mesh_width / block.x, mesh_height / block.y, 1);
    //simple_vbo_kernel<<< grid, block>>>(pos, mesh_width, mesh_height, time);
}

void runCuda(struct cudaGraphicsResource **vbo_resource)
{
    // map OpenGL buffer object for writing from CUDA
    float4 *dptr;
    checkCudaErrors(cudaGraphicsMapResources(1, vbo_resource, 0));
    size_t num_bytes;
    checkCudaErrors(cudaGraphicsResourceGetMappedPointer((void **)&dptr, &num_bytes,
                                                         *vbo_resource));
    //printf("CUDA mapped VBO: May access %ld bytes\n", num_bytes);

    // execute the kernel
    //    dim3 block(8, 8, 1);
    //    dim3 grid(mesh_width / block.x, mesh_height / block.y, 1);
    //    kernel<<< grid, block>>>(dptr, mesh_width, mesh_height, g_fAnim);

    launch_kernel(dptr, mesh_width, mesh_height, g_fAnim);

    // unmap buffer object
    checkCudaErrors(cudaGraphicsUnmapResources(1, vbo_resource, 0));
}

void createVBO(GLuint *vbo, struct cudaGraphicsResource **vbo_res,
               unsigned int vbo_res_flags)
{
    assert(vbo);

    // create buffer object
    glGenBuffers(1, vbo);
    glBindBuffer(GL_ARRAY_BUFFER, *vbo);

    // initialize buffer object
    unsigned int size = mesh_width * mesh_height * 4 * sizeof(float);
    glBufferData(GL_ARRAY_BUFFER, size, 0, GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // register this buffer object with CUDA
    checkCudaErrors(cudaGraphicsGLRegisterBuffer(vbo_res, *vbo, vbo_res_flags));

    SDK_CHECK_ERROR_GL();
}

void deleteVBO(GLuint *vbo, struct cudaGraphicsResource *vbo_res)
{

    // unregister this buffer object with CUDA
    cudaGraphicsUnregisterResource(vbo_res);

    glBindBuffer(1, *vbo);
    glDeleteBuffers(1, vbo);

    *vbo = 0;
}

void display()
{
    // run CUDA kernel to generate vertex positions
    runCuda(&cuda_vbo_resource);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // set view matrix
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslatef(0.0, 0.0, translate_z);
    glRotatef(rotate_x, 1.0, 0.0, 0.0);
    glRotatef(rotate_y, 0.0, 1.0, 0.0);

    // render from the vbo
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glVertexPointer(4, GL_FLOAT, 0, 0);

    glEnableClientState(GL_VERTEX_ARRAY);
    glColor3f(1.0, 0.0, 0.0);
    glDrawArrays(GL_POINTS, 0, mesh_width * mesh_height);
    glDisableClientState(GL_VERTEX_ARRAY);

    glutSwapBuffers();
}

int main(int argc, char **argv)
{
    // Create the CUTIL timer
    sdkCreateTimer(&timer);

    cudaGLSetGLDevice(gpuGetMaxGflopsDeviceId());

    // register callbacks
    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glutMouseFunc(mouse);
    glutMotionFunc(motion);

    glutCloseFunc(cleanup);

    // create VBO
    createVBO(&vbo, &cuda_vbo_resource, cudaGraphicsMapFlagsWriteDiscard);

    // run the cuda part
    runCuda(&cuda_vbo_resource);

    // start rendering mainloop
    glutMainLoop();

}