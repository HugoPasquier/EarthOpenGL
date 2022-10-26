#include "viewer.h"
#include "camera.h"
#include "SOIL2.h"

using namespace Eigen;

Viewer::Viewer()
  : _winWidth(0), _winHeight(0), _theta(0), _rotate(true)
{
}

Viewer::~Viewer()
{
}

////////////////////////////////////////////////////////////////////////////////
// GL stuff

// initialize OpenGL context
void Viewer::init(int w, int h){

    // Background color
    glClearColor(0.0, 0.0, 0.0, 0.0);

    // Texture
    image = SOIL_load_OGL_texture(DATA_DIR"/textures/earth.jpg", 3, 0, SOIL_FLAG_MIPMAPS);
    image2 = SOIL_load_OGL_texture(DATA_DIR"/textures/earth_clouds.jpg", 3, 0, SOIL_FLAG_MIPMAPS);
    image3 = SOIL_load_OGL_texture(DATA_DIR"/textures/earth_night.jpg", 3, 0, SOIL_FLAG_MIPMAPS);
    image4 = SOIL_load_OGL_texture(DATA_DIR"/textures/earth_normal.jpg", 3, 0, SOIL_FLAG_MIPMAPS);

    glGenSamplers(1, &_samplerId);


    loadShaders();
    if(!_mesh.load(DATA_DIR"/models/earth.obj")) exit(1);
    _mesh.initVBA();

    reshape(w,h);
    _cam.setPerspective(M_PI/3,0.1f,20000.0f);
    _cam.lookAt(Vector3f(0,0,4), Vector3f(0,0,0), Vector3f(0,1,0));
    _trackball.setCamera(&_cam);

    glEnable(GL_DEPTH_TEST);
}

void Viewer::reshape(int w, int h){
    _winWidth = w;
    _winHeight = h;
    _cam.setViewport(w,h);
}


/*!
   callback to draw graphic primitives
 */
void Viewer::drawScene()
{
    // configure the rendering target size (viewport)
    glViewport(0, 0, _winWidth, _winHeight);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    _shader.activate();

    // Textures 
    // image 1
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, image);
    glUniform1i(_shader.getUniformLocation("image"),0);

    // glBindSampler(0, _samplerId);
    // glSamplerParameteri(_samplerId, _minFilter, _magFilter);

    // image 2 (nuages)
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, image2);
    glUniform1i(_shader.getUniformLocation("image2"),1);

    // glBindSampler(1, _samplerId);
    // glSamplerParameteri(_samplerId, _minFilter, _magFilter);

    // image 3 (nuages)
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, image3);
    glUniform1i(_shader.getUniformLocation("image3"),2);

    // image 4 (normal map)
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, image4);
    glUniform1i(_shader.getUniformLocation("image4"),3);

    // glBindSampler(2, _samplerId);
    // glSamplerParameteri(_samplerId, _minFilter, _magFilter);


    glUniformMatrix4fv(_shader.getUniformLocation("view_mat"),1,GL_FALSE,_cam.viewMatrix().data());
    glUniformMatrix4fv(_shader.getUniformLocation("proj_mat"),1,GL_FALSE,_cam.projectionMatrix().data());

    Affine3f M(AngleAxisf(_theta,Vector3f(0,1,0)));

    glUniformMatrix4fv(_shader.getUniformLocation("obj_mat"),1,GL_FALSE,M.matrix().data());

    Matrix4f matLocal2Cam = _cam.viewMatrix() * M.matrix();
    Matrix3f matN = matLocal2Cam.topLeftCorner<3,3>().inverse().transpose();
    glUniformMatrix3fv(_shader.getUniformLocation("normal_mat"),1,GL_FALSE,matN.data());

    Vector3f lightDir = Vector3f(1,0,1).normalized();
    lightDir = (matLocal2Cam.topLeftCorner<3,3>() * lightDir).normalized();
    glUniform3fv(_shader.getUniformLocation("lightDir"),1,lightDir.data());

    _mesh.draw(_shader);

    _shader.deactivate();
}


void Viewer::updateAndDrawScene()
{
  if(_rotate)
    _theta += 0.01*M_PI;
  drawScene();
}

void Viewer::loadShaders()
{
    // Here we can load as many shaders as we want, currently we have only one:
    _shader.loadFromFiles(DATA_DIR"/shaders/simple.vert", DATA_DIR"/shaders/simple.frag");
    checkError();
}

////////////////////////////////////////////////////////////////////////////////
// Events

/*!
   callback to manage mouse : called when user press or release mouse button
   You can change in this function the way the user
   interact with the application.
 */
void Viewer::mousePressed(GLFWwindow */*window*/, int /*button*/, int action)
{
  if(action == GLFW_PRESS)
  {
      _trackingMode = TM_ROTATE_AROUND;
      _trackball.start();
      _trackball.track(_lastMousePos);
  }
  else if(action == GLFW_RELEASE)
  {
      _trackingMode = TM_NO_TRACK;
  }
}


/*!
   callback to manage mouse : called when user move mouse with button pressed
   You can change in this function the way the user
   interact with the application.
 */
void Viewer::mouseMoved(int x, int y)
{
    if(_trackingMode == TM_ROTATE_AROUND)
    {
        _trackball.track(Vector2i(x,y));
    }

    _lastMousePos = Vector2i(x,y);
}

void Viewer::mouseScroll(double /*x*/, double y)
{
  _cam.zoom(-0.1*y);
}

/*!
   callback to manage keyboard interactions
   You can change in this function the way the user
   interact with the application.
 */
void Viewer::keyPressed(int key, int action, int /*mods*/)
{
  if(key == GLFW_KEY_R && action == GLFW_PRESS)
  {
    loadShaders();
  }
  else if(key == GLFW_KEY_A && action == GLFW_PRESS)
  {
    _rotate = !_rotate;
  }

  if(action == GLFW_PRESS || action == GLFW_REPEAT )
  {
    if (key==GLFW_KEY_UP)
    {
      if(_magFilter == GL_NEAREST)
        _magFilter = GL_LINEAR;
      else 
        _magFilter = GL_NEAREST;
    }
    else if (key==GLFW_KEY_DOWN)
    {
      switch (_minFilter){
        case GL_NEAREST:
        _minFilter = GL_LINEAR;
        break;
        case GL_LINEAR:
        _minFilter = GL_NEAREST_MIPMAP_NEAREST;
        break;
        case GL_NEAREST_MIPMAP_NEAREST:
        _minFilter = GL_NEAREST_MIPMAP_LINEAR;
        break;
        case GL_NEAREST_MIPMAP_LINEAR:
        _minFilter = GL_LINEAR_MIPMAP_LINEAR;
        break;
        case GL_LINEAR_MIPMAP_LINEAR:
        _minFilter = GL_LINEAR_MIPMAP_NEAREST;
        break;
        case GL_LINEAR_MIPMAP_NEAREST:
        _minFilter = GL_NEAREST;
        break;


      
      default:
        break;
      }
    }
    else if (key==GLFW_KEY_LEFT)
    {
    }
    else if (key==GLFW_KEY_RIGHT)
    {
    }
    else if (key==GLFW_KEY_PAGE_UP)
    {
    }
    else if (key==GLFW_KEY_PAGE_DOWN)
    {
    }
  }
}

void Viewer::charPressed(int /*key*/)
{
}
