#ifndef GL_UTILS_H
#define GL_UTILS_H
#include<QtOpenGL/QtOpenGL>

class ShaderHelper
{
public:

    static int getProgramByShader(std::string vertex_shader_name, std::string fragment_shader_name);

    static int linkProgram(int vertex_shader_id, int fragment_shader_id);

    static int complieShader(GLenum shader_type, std::string shader_code);
};

class FileHelper
{
public:
    FileHelper() {}
        static std::shared_ptr<std::string> readShaderFile(std::string file_name);
};

#endif // GL_UTILS_H

int ShaderHelper::getProgramByShader(std::string vertex_shader_name, std::string fragment_shader_name)
{
    auto vertex_shader_code = FileHelper::readShaderFile(vertex_shader);
     auto fragment_shader_code = FileHelper::readShaderFile(fragment_shader);

     auto vertex_shader_id = complieShader(GL_VERTEX_SHADER, *vertex_shader_code);
     auto fragment_shader_id = complieShader(GL_FRAGMENT_SHADER, *fragment_shader_code);


     return linkProgram(vertex_shader_id, fragment_shader_id);
}

int ShaderHelper::linkProgram(int vertex_shader_id, int fragment_shader_id)
{
    auto programId = glCreateProgram();
      if (programId == 0) {
          std::cout << "create program failed" << glGetError() << std::endl;

          return 0;
      }
      glAttachShader(programId, vertex_shader_id);
      glAttachShader(programId, fragment_shader_id);
      glLinkProgram(programId);

      auto link_status = new int[1];
      glGetProgramiv(programId, GL_LINK_STATUS, link_status);
      if (link_status[0] == 0) {
          glDeleteProgram(programId);
          return 0;
      }
      return programId;
}

int ShaderHelper::complieShader(GLenum shader_type, std::string shader_code)
{
    int shaderId = glCreateShader(shader_type);


      if (shaderId == 0) {
          std::cout << "create shader failed" << glGetError() << std::endl;
          return 0;
      }
      glShaderSource(shaderId, 1, (const char **) &shader_code, NULL);
      glCompileShader(shaderId);
      GLint *complie_status = new GLint[1];
      glGetShaderiv(shaderId, GL_COMPILE_STATUS, complie_status);
      if (complie_status[0] == 0) {
          glDeleteShader(shaderId);
          return 0;
      }
      delete complie_status;

      return shaderId;
}

std::shared_ptr<std::string> FileHelper::readShaderFile(std::string file_name)
{
#ifdef __WIN32__
#define ROOT_PATH "E:\\workspace\\Opengl\\OpenglStudio\\shaders\\"
#else
#define ROOT_PATH "/root/workspace/OpenGL/OpenglStudio/shaders/"
#endif

    std::string file_path = ROOT_PATH + file_name + ".glsl";
    std::ifstream find(file_path);
    if (!find) {
        std::cout << "file " << file_path << " is not exist！" << std::endl;
        return nullptr;
    }

    std::ifstream infile;
    infile.open(file_path.data());
    std::string line, result;

    while (getline(infile, line)) {
        result += line;
    }
    infile.close();
    auto p_result = std::make_shared<std::string>(result);
    return p_result;
}
