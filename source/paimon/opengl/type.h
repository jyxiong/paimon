#pragma once

#include <glad/gl.h>

namespace paimon {

enum class ObjectIdentifier : GLenum {
  Buffer = GL_BUFFER,
  Texture = GL_TEXTURE,
  Shader = GL_SHADER,
  Program = GL_PROGRAM,
  VertexArray = GL_VERTEX_ARRAY,
  Framebuffer = GL_FRAMEBUFFER,
};

// data
enum class InternalFormat : GLenum {
  R8 = GL_R8,
  R16 = GL_R16,
  RG8 = GL_RG8,
  RG16 = GL_RG16,
  RGB8 = GL_RGB8,
  RGB16 = GL_RGB16,
  RGBA8 = GL_RGBA8,
  RGBA16 = GL_RGBA16,

  R8SN = GL_R8_SNORM,
  R16SN = GL_R16_SNORM,
  RG8SN = GL_RG8_SNORM,
  RG16SN = GL_RG16_SNORM,
  RGB8SN = GL_RGB8_SNORM,
  RGB16SN = GL_RGB16_SNORM,
  RGBA8SN = GL_RGBA8_SNORM,
  RGBA16SN = GL_RGBA16_SNORM,

  R8I = GL_R8I,
  R16I = GL_R16I,
  R32I = GL_R32I,
  RG8I = GL_RG8I,
  RG16I = GL_RG16I,
  RG32I = GL_RG32I,
  RGB8I = GL_RGB8I,
  RGB16I = GL_RGB16I,
  RGB32I = GL_RGB32I,
  RGBA8I = GL_RGBA8I,
  RGBA16I = GL_RGBA16I,
  RGBA32I = GL_RGBA32I,

  R8UI = GL_R8UI,
  R16UI = GL_R16UI,
  R32UI = GL_R32UI,
  RG8UI = GL_RG8UI,
  RG16UI = GL_RG16UI,
  RG32UI = GL_RG32UI,
  RGB8UI = GL_RGB8UI,
  RGB16UI = GL_RGB16UI,
  RGB32UI = GL_RGB32UI,
  RGBA8UI = GL_RGBA8UI,
  RGBA16UI = GL_RGBA16UI,
  RGBA32UI = GL_RGBA32UI,

  R16F = GL_R16F,
  R32F = GL_R32F,
  RG16F = GL_RG16F,
  RG32F = GL_RG32F,
  RGB16F = GL_RGB16F,
  RGB32F = GL_RGB32F,
  RGBA16F = GL_RGBA16F,
  RGBA32F = GL_RGBA32F,

  Depth16 = GL_DEPTH_COMPONENT16,
  Depth24 = GL_DEPTH_COMPONENT24,
  Depth32F = GL_DEPTH_COMPONENT32F,
  Depth24Stencil8 = GL_DEPTH24_STENCIL8,
  Depth32FStencil8 = GL_DEPTH32F_STENCIL8,
};

enum class DataFormat : GLenum {
  R = GL_RED,
  RG = GL_RG,
  RGB = GL_RGB,
  BGR = GL_BGR,
  RGBA = GL_RGBA,
  BGRA = GL_BGRA,

  DepthComponent = GL_DEPTH_COMPONENT,
  DepthStencil = GL_DEPTH_STENCIL,

  StencilIndex = GL_STENCIL_INDEX,
};

enum class DataType : GLenum {
  UByte = GL_UNSIGNED_BYTE,
  Byte = GL_BYTE,
  UShort = GL_UNSIGNED_SHORT,
  Short = GL_SHORT,
  UInt = GL_UNSIGNED_INT,
  Int = GL_INT,
  Float = GL_FLOAT,
  Double = GL_DOUBLE,
};

// buffer
enum class BufferUsageBit : GLbitfield {
  ClientStorageBit = GL_CLIENT_STORAGE_BIT,
  DynamicStorageBit = GL_DYNAMIC_STORAGE_BIT,
  MapReadBit = GL_MAP_READ_BIT,
  MapWriteBit = GL_MAP_WRITE_BIT,
  MapPersistentBit = GL_MAP_PERSISTENT_BIT,
  MapCoherentBit = GL_MAP_COHERENT_BIT,
};

enum class BufferAccess : GLbitfield {
  StreamDraw = GL_STREAM_DRAW,
  StreamRead = GL_STREAM_READ,
  StreamCopy = GL_STREAM_COPY,
  StaticDraw = GL_STATIC_DRAW,
  StaticRead = GL_STATIC_READ,
  StaticCopy = GL_STATIC_COPY,
  DynamicDraw = GL_DYNAMIC_DRAW,
  DynamicRead = GL_DYNAMIC_READ,
  DynamicCopy = GL_DYNAMIC_COPY,
};

enum class BufferTarget : GLenum {
  ArrayBuffer = GL_ARRAY_BUFFER,
  AtomicCounterBuffer = GL_ATOMIC_COUNTER_BUFFER,
  CopyReadBuffer = GL_COPY_READ_BUFFER,
  CopyWriteBuffer = GL_COPY_WRITE_BUFFER,
  DispatchIndirectBuffer = GL_DISPATCH_INDIRECT_BUFFER,
  DrawIndirectBuffer = GL_DRAW_INDIRECT_BUFFER,
  ElementArrayBuffer = GL_ELEMENT_ARRAY_BUFFER,
  PixelPackBuffer = GL_PIXEL_PACK_BUFFER,
  PixelUnpackBuffer = GL_PIXEL_UNPACK_BUFFER,
  QueryBuffer = GL_QUERY_BUFFER,
  ShaderStorageBuffer = GL_SHADER_STORAGE_BUFFER,
  TextureBuffer = GL_TEXTURE_BUFFER,
  TransformFeedbackBuffer = GL_TRANSFORM_FEEDBACK_BUFFER,
  UniformBuffer = GL_UNIFORM_BUFFER,
};

// texture
enum class TextureTarget : GLenum {
  Texture1D = GL_TEXTURE_1D,
  Texture2D = GL_TEXTURE_2D,
  Texture3D = GL_TEXTURE_3D,
  Texture1DArray = GL_TEXTURE_1D_ARRAY,
  Texture2DArray = GL_TEXTURE_2D_ARRAY,
  TextureCubeMap = GL_TEXTURE_CUBE_MAP,
  TextureCubeMapArray = GL_TEXTURE_CUBE_MAP_ARRAY,
  Texture2DMultisample = GL_TEXTURE_2D_MULTISAMPLE,
  Texture2DMultisampleArray = GL_TEXTURE_2D_MULTISAMPLE_ARRAY,
  TextureBuffer = GL_TEXTURE_BUFFER,
  TextureRectangle = GL_TEXTURE_RECTANGLE,
};

enum class TextureWrapMode : GLenum {
  ClampToEdge = GL_CLAMP_TO_EDGE,
  MirroredRepeat = GL_MIRRORED_REPEAT,
  Repeat = GL_REPEAT,
  ClampToBorder = GL_CLAMP_TO_BORDER,
};

enum class TextureFilterMode : GLenum {
  Nearest = GL_NEAREST,
  Linear = GL_LINEAR,
  NearestMipmapNearest = GL_NEAREST_MIPMAP_NEAREST,
  LinearMipmapNearest = GL_LINEAR_MIPMAP_NEAREST,
  NearestMipmapLinear = GL_NEAREST_MIPMAP_LINEAR,
  LinearMipmapLinear = GL_LINEAR_MIPMAP_LINEAR,
};

enum class ImageAccess : GLenum {
  ReadOnly = GL_READ_ONLY,
  WriteOnly = GL_WRITE_ONLY,
  ReadWrite = GL_READ_WRITE,
};

// framebuffer
enum class FramebufferTarget : GLenum {
  Framebuffer = GL_FRAMEBUFFER,
  DrawFramebuffer = GL_DRAW_FRAMEBUFFER,
  ReadFramebuffer = GL_READ_FRAMEBUFFER,
};

enum class FramebufferAttachmentPoint : GLenum {
  ColorAttachment0 = GL_COLOR_ATTACHMENT0,
  ColorAttachment1 = GL_COLOR_ATTACHMENT1,
  ColorAttachment2 = GL_COLOR_ATTACHMENT2,
  ColorAttachment3 = GL_COLOR_ATTACHMENT3,
  DepthAttachment = GL_DEPTH_ATTACHMENT,
  StencilAttachment = GL_STENCIL_ATTACHMENT,
  DepthStencilAttachment = GL_DEPTH_STENCIL_ATTACHMENT,
};

// shader
enum class ShaderType : GLenum {
  VertexShader = GL_VERTEX_SHADER,
  TessControlShader = GL_TESS_CONTROL_SHADER,
  TessEvaluationShader = GL_TESS_EVALUATION_SHADER,
  GeometryShader = GL_GEOMETRY_SHADER,
  FragmentShader = GL_FRAGMENT_SHADER,
  ComputeShader = GL_COMPUTE_SHADER,
};

enum class ProgramStages : GLbitfield {
  VertexShaderBit = GL_VERTEX_SHADER_BIT,
  TessControlShaderBit = GL_TESS_CONTROL_SHADER_BIT,
  TessEvaluationShaderBit = GL_TESS_EVALUATION_SHADER_BIT,
  GeometryShaderBit = GL_GEOMETRY_SHADER_BIT,
  FragmentShaderBit = GL_FRAGMENT_SHADER_BIT,
  ComputeShaderBit = GL_COMPUTE_SHADER_BIT,
};

enum class PrimitiveTopology : GLenum {
  Points = GL_POINTS,
  Lines = GL_LINES,
  LineAdjacency = GL_LINES_ADJACENCY,
  LinesLoop = GL_LINE_LOOP,
  LineStrip = GL_LINE_STRIP,
  LineStripAdjacency = GL_LINE_STRIP_ADJACENCY,
  Triangles = GL_TRIANGLES,
  TrianglesAdjacency = GL_TRIANGLES_ADJACENCY,
  TriangleFan = GL_TRIANGLE_FAN,
  TriangleStrip = GL_TRIANGLE_STRIP,
  TriangleStripAdjacency = GL_TRIANGLE_STRIP_ADJACENCY,
};

template <class T>
GLenum cast_enum(T value) {
  return static_cast<GLenum>(value);
}

template <class T>
GLbitfield cast_bitfield(T value) {
  return static_cast<GLbitfield>(value);
}

template <class T>
GLsizei cast_sizei(T value) {
  return static_cast<GLsizei>(value);
}

template <class T>
GLsizeiptr cast_sizeiptr(T value) {
  return static_cast<GLsizeiptr>(value);
}

template <class T>
GLintptr cast_intptr(T value) {
  return static_cast<GLintptr>(value);
}

} // namespace paimon