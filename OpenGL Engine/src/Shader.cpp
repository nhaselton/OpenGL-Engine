#include "Shader.h"

Shader::Shader( const char* vertexPath, const char* fragmentPath) {
    Create( vertexPath, fragmentPath );
}

Shader::Shader( const char* path , bool geoShader) {
    Create( path , geoShader);
}

void Shader::Create( const std::string& vertexPath, const std::string& fragmentPath, const std::string& geoPath ) {
    // 1. retrieve the vertex/fragment source code from filePath
    std::string vertexCode;
    std::string fragmentCode;
    std::string geoCode;
    std::ifstream vShaderFile;
    std::ifstream fShaderFile;
    std::ifstream gShaderFile;

    bool hasGeo = ( geoPath != "none" );

    // ensure ifstream objects can throw exceptions:
    vShaderFile.exceptions( std::ifstream::failbit | std::ifstream::badbit );
    fShaderFile.exceptions( std::ifstream::failbit | std::ifstream::badbit );
    gShaderFile.exceptions( std::ifstream::failbit | std::ifstream::badbit );
    try {
        // open files
        vShaderFile.open( vertexPath );
        fShaderFile.open( fragmentPath );
        if ( hasGeo )
            gShaderFile.open(geoPath);
        

        std::stringstream vShaderStream, fShaderStream,gShaderStream;
        // read file's buffer contents into streams
        vShaderStream << vShaderFile.rdbuf();
        fShaderStream << fShaderFile.rdbuf();
        
        if ( hasGeo )
            gShaderStream << gShaderFile.rdbuf();
        // close file handlers
        vShaderFile.close();
        fShaderFile.close();
        
        if ( hasGeo )
            gShaderFile.close();
        // convert stream into string
        vertexCode = vShaderStream.str();
        fragmentCode = fShaderStream.str();


        if ( hasGeo )
            geoCode = gShaderStream.str();
            // if geometry shader path is present, also load a geometry shader
    }

    catch ( std::ifstream::failure& e ) {
        std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
        std::cout << "Path: " << vertexPath << std::endl;
    }

    const char* vShaderCode = vertexCode.c_str();
    const char* fShaderCode = fragmentCode.c_str();
    const char* gShaderCode = ( hasGeo ) ? geoCode.c_str() : "";
    // 2. compile shaders
    unsigned int vertex, fragment;
    // vertex shader
    vertex = glCreateShader( GL_VERTEX_SHADER );
    glShaderSource( vertex, 1, &vShaderCode, NULL );
    glCompileShader( vertex );
    checkCompileErrors( vertex, "VERTEX" );
    // fragment Shader
    fragment = glCreateShader( GL_FRAGMENT_SHADER );
    glShaderSource( fragment, 1, &fShaderCode, NULL );
    glCompileShader( fragment );
    checkCompileErrors( fragment, "FRAGMENT" );
    // if geometry shader is given, compile geometry shader

    // shader Program
    ID = glCreateProgram();
    glAttachShader( ID, vertex );
    glAttachShader( ID, fragment );
 
    //Try to create geo shader
    if ( hasGeo ) {
        unsigned int geometry;
        geometry = glCreateShader( GL_GEOMETRY_SHADER );
        glShaderSource( geometry, 1, &gShaderCode, NULL );
        glCompileShader( geometry );
        checkCompileErrors( geometry, "VERTEX" );
        glAttachShader( ID, geometry );
    }
    glLinkProgram( ID );
    checkCompileErrors( ID, "PROGRAM" );
    // delete the shaders as they're linked into our program now and no longer necessery
    glDeleteShader( vertex );
    glDeleteShader( fragment );
    
}

void Shader::Create( const std::string& directory, bool hasGeoShader ) {
    //Get Name of shader
    std::string name = directory.substr( directory.find_last_of( '/' ), directory.length() - 1 );
    //Create both .vs & .fs version of it 
    std::string vertexPath = directory + name + ".vs";
    std::string fragmentPath = directory + name + ".fs";
    std::string geoShader = "none";
    if ( hasGeoShader )
        geoShader = directory + name + ".gs";
    //Run Normal Create Method
    Create( vertexPath, fragmentPath ,geoShader);
}

void Shader::checkCompileErrors( GLuint modelBoneShader, std::string type ) {
    GLint success;
    GLchar infoLog[1024];
    if ( type != "PROGRAM" ) {
        glGetShaderiv( modelBoneShader, GL_COMPILE_STATUS, &success );
        if ( !success ) {
            glGetShaderInfoLog( modelBoneShader, 1024, NULL, infoLog );
            std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
        }
    }
    else {
        glGetProgramiv( modelBoneShader, GL_LINK_STATUS, &success );
        if ( !success ) {
            glGetProgramInfoLog( modelBoneShader, 1024, NULL, infoLog );
            std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
        }
    }
}

Shader::Shader( const char* vertexPath, const char* fragmentPath, const char* geometryPath, int a, int b )
{
    // 1. retrieve the vertex/fragment source code from filePath
    std::string vertexCode;
    std::string fragmentCode;
    std::string geometryCode;
    std::ifstream vShaderFile;
    std::ifstream fShaderFile;
    std::ifstream gShaderFile;
    // ensure ifstream objects can throw exceptions:
    vShaderFile.exceptions( std::ifstream::failbit | std::ifstream::badbit );
    fShaderFile.exceptions( std::ifstream::failbit | std::ifstream::badbit );
    gShaderFile.exceptions( std::ifstream::failbit | std::ifstream::badbit );
    try
    {
        // open files
        vShaderFile.open( vertexPath );
        fShaderFile.open( fragmentPath );
        std::stringstream vShaderStream, fShaderStream;
        // read file's buffer contents into streams
        vShaderStream << vShaderFile.rdbuf();
        fShaderStream << fShaderFile.rdbuf();
        // close file handlers
        vShaderFile.close();
        fShaderFile.close();
        // convert stream into string
        vertexCode = vShaderStream.str();
        fragmentCode = fShaderStream.str();
        // if geometry shader path is present, also load a geometry shader
        if ( geometryPath != nullptr )
        {
            gShaderFile.open( geometryPath );
            std::stringstream gShaderStream;
            gShaderStream << gShaderFile.rdbuf();
            gShaderFile.close();
            geometryCode = gShaderStream.str();
        }
    }
    catch ( std::ifstream::failure& e )
    {
        std::cout << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ: " << e.what() << std::endl;
    }
    const char* vShaderCode = vertexCode.c_str();
    const char* fShaderCode = fragmentCode.c_str();
    // 2. compile shaders
    unsigned int vertex, fragment;
    // vertex shader
    vertex = glCreateShader( GL_VERTEX_SHADER );
    glShaderSource( vertex, 1, &vShaderCode, NULL );
    glCompileShader( vertex );
    checkCompileErrors( vertex, "VERTEX" );
    // fragment Shader
    fragment = glCreateShader( GL_FRAGMENT_SHADER );
    glShaderSource( fragment, 1, &fShaderCode, NULL );
    glCompileShader( fragment );
    checkCompileErrors( fragment, "FRAGMENT" );
    // if geometry shader is given, compile geometry shader
    unsigned int geometry;
    if ( geometryPath != nullptr )
    {
        const char* gShaderCode = geometryCode.c_str();
        geometry = glCreateShader( GL_GEOMETRY_SHADER );
        glShaderSource( geometry, 1, &gShaderCode, NULL );
        glCompileShader( geometry );
        checkCompileErrors( geometry, "GEOMETRY" );
    }
    // shader Program
    ID = glCreateProgram();
    glAttachShader( ID, vertex );
    glAttachShader( ID, fragment );
    if ( geometryPath != nullptr )
        glAttachShader( ID, geometry );
    glLinkProgram( ID );
    checkCompileErrors( ID, "PROGRAM" );
    // delete the shaders as they're linked into our program now and no longer necessary
    glDeleteShader( vertex );
    glDeleteShader( fragment );
    if ( geometryPath != nullptr )
        glDeleteShader( geometry );

}
// activate the shader
