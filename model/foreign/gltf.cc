#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION

#include "gltf.h"
#include "../animation/clip.h"
#include "../animation/frame.h"
#include "../animation/pose.h"
#include "../animation/skeleton.h"
#include "../model.h"
#include "../renderer/mesh.h"

GLTFFile::GLTFFile(std::string &path)
{
  tinygltf::TinyGLTF loader;
  std::string err, warn;

  std::cout << "Loading GLTF file: " << path << std::endl;

  if (path.substr(path.find_last_of(".") + 1) == "gltf")
  {
    if (!loader.LoadASCIIFromFile(&this->tinyModel, &err, &warn, path))
    {
      std::cerr << "Failed to load GLTF file: " << path << std::endl;
      std::cerr << "Warning: " << warn << std::endl;
      std::cerr << "Error: " << err << std::endl;
      throw std::runtime_error(warn + err);
    }
  }
  else if (path.substr(path.find_last_of(".") + 1) == "glb")
  {
    if (!loader.LoadBinaryFromFile(&this->tinyModel, &err, &warn, path))
    {
      std::cerr << "Failed to load GLB file: " << path << std::endl;
      std::cerr << "Warning: " << warn << std::endl;
      std::cerr << "Error: " << err << std::endl;
      throw std::runtime_error(warn + err);
    }
  }
  else
  {
    std::runtime_error("not a gltf or glb file!");
  }
}

void GLTFFile::populateModel(Model &model)
{
  model.meshes = this->getMeshes();

  model.textures = this->getTextures();

  Skeleton skeleton;
  skeleton = this->getSkeleton();
  if (skeleton.restPose.size() > 0)
  {
    model.animController = new Controller();
    model.animController->setSkeleton(new Skeleton(skeleton));
  }

  std::vector<Clip> clips;
  clips = this->getClips();

  if (model.animController == nullptr && clips.size() > 0)
  {
    std::cout << "Warning: model has animation clips but no skeleton\n";
  }

  if (model.animController != nullptr)
  {
    for (auto clip : clips)
    {
      model.animController->addClip(new Clip(clip));
    }
  }

  model.normalize();
}

template <typename T>
const T *getData(const tinygltf::Model &tinyModel, const int index);

std::vector<Mesh> GLTFFile::getMeshes()
{
  std::vector<Mesh> meshes;

  for (size_t m = 0; m < this->tinyModel.meshes.size(); ++m)
  {
    tinygltf::Mesh &mesh = this->tinyModel.meshes[m];

    for (size_t j = 0; j < mesh.primitives.size(); ++j)
    {
      Mesh tmpmesh = {};
      tmpmesh.mode = TRIANGLES;

      tinygltf::Primitive &primitive = mesh.primitives[j];
      // positions
      auto it = primitive.attributes.find("POSITION");
      if (it != primitive.attributes.end())
      {
        const tinygltf::Accessor &accessor = tinyModel.accessors[it->second];
        int count = accessor.count;

        Vector3f pos;
        for (size_t i = 0; i < count; ++i)
        {
          // Handle different component types for positions
          switch (accessor.componentType)
          {
          case TINYGLTF_COMPONENT_TYPE_FLOAT:
          {
            const float *data = getData<float>(tinyModel, it->second);
            pos = Vector3f(data[i * 3], data[i * 3 + 1], data[i * 3 + 2]);
            break;
          }
          case TINYGLTF_COMPONENT_TYPE_BYTE:
          {
            const int8_t *data = getData<int8_t>(tinyModel, it->second);
            pos = Vector3f(data[i * 3] / 127.0f, data[i * 3 + 1] / 127.0f, data[i * 3 + 2] / 127.0f);
            break;
          }
          case TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE:
          {
            const uint8_t *data = getData<uint8_t>(tinyModel, it->second);
            pos = Vector3f(data[i * 3] / 255.0f, data[i * 3 + 1] / 255.0f, data[i * 3 + 2] / 255.0f);
            break;
          }
          case TINYGLTF_COMPONENT_TYPE_SHORT:
          {
            const int16_t *data = getData<int16_t>(tinyModel, it->second);
            pos = Vector3f(data[i * 3] / 32767.0f, data[i * 3 + 1] / 32767.0f, data[i * 3 + 2] / 32767.0f);
            break;
          }
          case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT:
          {
            const uint16_t *data = getData<uint16_t>(tinyModel, it->second);
            pos = Vector3f(data[i * 3] / 65535.0f, data[i * 3 + 1] / 65535.0f, data[i * 3 + 2] / 65535.0f);
            break;
          }
          default:
            std::cerr << "Unsupported position component type: " << accessor.componentType << std::endl;
            pos = Vector3f(0, 0, 0);
          }

          Vertex vertex = {.pos = pos};
          tmpmesh.vertices.push_back(vertex);
        }
      }
      else
      {
        std::cout << "no position attribute found in primitive " << j
                  << " of mesh " << m << "\n";
      }

      // normals
      it = primitive.attributes.find("NORMAL");
      if (it != primitive.attributes.end())
      {
        const tinygltf::Accessor &accessor = tinyModel.accessors[it->second];
        int count = accessor.count;
        Vector3f norm;
        for (size_t i = 0; i < count; ++i)
        {
          // Handle different component types for normals
          switch (accessor.componentType)
          {
          case TINYGLTF_COMPONENT_TYPE_FLOAT:
          {
            const float *data = getData<float>(tinyModel, it->second);
            norm = Vector3f(data[i * 3], data[i * 3 + 1], data[i * 3 + 2]);
            break;
          }
          case TINYGLTF_COMPONENT_TYPE_BYTE:
          {
            const int8_t *data = getData<int8_t>(tinyModel, it->second);
            norm = Vector3f(data[i * 3] / 127.0f, data[i * 3 + 1] / 127.0f, data[i * 3 + 2] / 127.0f);
            break;
          }
          case TINYGLTF_COMPONENT_TYPE_SHORT:
          {
            const int16_t *data = getData<int16_t>(tinyModel, it->second);
            norm = Vector3f(data[i * 3] / 32767.0f, data[i * 3 + 1] / 32767.0f, data[i * 3 + 2] / 32767.0f);
            break;
          }
          default:
            std::cerr << "Unsupported normal component type: " << accessor.componentType << std::endl;
            norm = Vector3f(0, 0, 0);
          }

          tmpmesh.vertices[i].norm = norm;
        }
      }
      else
      {
        std::cout << "no normal attribute found in primitive " << j
                  << " of mesh " << m << "\n";
      }

      // texture coords
      it = primitive.attributes.find("TEXCOORD_0");
      if (it != primitive.attributes.end())
      {
        const tinygltf::Accessor &accessor = tinyModel.accessors[it->second];
        int count = accessor.count;

        Vector2f uv;
        for (size_t i = 0; i < count; ++i)
        {
          // Handle different component types for UVs
          switch (accessor.componentType)
          {
          case TINYGLTF_COMPONENT_TYPE_FLOAT:
          {
            const float *data = getData<float>(tinyModel, it->second);
            uv = Vector2f(data[i * 2], data[i * 2 + 1]);
            break;
          }
          case TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE:
          {
            const uint8_t *data = getData<uint8_t>(tinyModel, it->second);
            uv = Vector2f(data[i * 2] / 255.0f, data[i * 2 + 1] / 255.0f);
            break;
          }
          case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT:
          {
            const uint16_t *data = getData<uint16_t>(tinyModel, it->second);
            uv = Vector2f(data[i * 2] / 65535.0f, data[i * 2 + 1] / 65535.0f);
            break;
          }
          default:
            std::cerr << "Unsupported texcoord component type: " << accessor.componentType << std::endl;
            uv = Vector2f(0, 0);
          }

          tmpmesh.vertices[i].tc = uv;
        }
      }
      else
      {
        std::cout << "no texture coordinate attribute found in primitive " << j
                  << " of mesh " << m << "\n";
      }

      it = primitive.attributes.find("JOINTS_0");
      if (it != primitive.attributes.end())
      {
        const tinygltf::Accessor &accessor = tinyModel.accessors[it->second];
        int count = accessor.count;

        std::vector<int> skinjoints;
        skinjoints = tinyModel.skins[0].joints;

        for (size_t i = 0; i < count; ++i)
        {
          int joint_indices[4] = {0, 0, 0, 0};

          // Handle different component types for joints
          switch (accessor.componentType)
          {
          case TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE:
          {
            const uint8_t *data = getData<uint8_t>(tinyModel, it->second);
            for (int j = 0; j < 4; ++j)
              joint_indices[j] = data[i * 4 + j];
            break;
          }
          case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT:
          {
            const uint16_t *data = getData<uint16_t>(tinyModel, it->second);
            for (int j = 0; j < 4; ++j)
              joint_indices[j] = data[i * 4 + j];
            break;
          }
          default:
            std::cerr << "Unsupported joint component type: " << accessor.componentType << std::endl;
          }

          tmpmesh.vertices[i].joints[0] = skinjoints[joint_indices[0]];
          tmpmesh.vertices[i].joints[1] = skinjoints[joint_indices[1]];
          tmpmesh.vertices[i].joints[2] = skinjoints[joint_indices[2]];
          tmpmesh.vertices[i].joints[3] = skinjoints[joint_indices[3]];
        };
      }
      else
      {
        std::cout << "no joints attribute found in primitive " << j
                  << " of mesh " << m << "\n";
      }

      it = primitive.attributes.find("WEIGHTS_0");
      if (it != primitive.attributes.end())
      {
        const tinygltf::Accessor &accessor = tinyModel.accessors[it->second];
        int count = accessor.count;

        for (size_t i = 0; i < count; ++i)
        {
          float weights[4] = {0.0f, 0.0f, 0.0f, 0.0f};

          // Handle different component types for weights
          switch (accessor.componentType)
          {
          case TINYGLTF_COMPONENT_TYPE_FLOAT:
          {
            const float *data = getData<float>(tinyModel, it->second);
            for (int j = 0; j < 4; ++j)
              weights[j] = data[i * 4 + j];
            break;
          }
          case TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE:
          {
            const uint8_t *data = getData<uint8_t>(tinyModel, it->second);
            for (int j = 0; j < 4; ++j)
              weights[j] = data[i * 4 + j] / 255.0f;
            break;
          }
          case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT:
          {
            const uint16_t *data = getData<uint16_t>(tinyModel, it->second);
            for (int j = 0; j < 4; ++j)
              weights[j] = data[i * 4 + j] / 65535.0f;
            break;
          }
          default:
            std::cerr << "Unsupported weight component type: " << accessor.componentType << std::endl;
          }

          tmpmesh.vertices[i].weights[0] = weights[0];
          tmpmesh.vertices[i].weights[1] = weights[1];
          tmpmesh.vertices[i].weights[2] = weights[2];
          tmpmesh.vertices[i].weights[3] = weights[3];
        };
      }
      else
      {
        std::cout << "no weights attributes found in primitive " << j
                  << " of mesh " << m << "\n";
      }

      if (primitive.indices >= 0)
      {
        const tinygltf::Accessor &accessor = tinyModel.accessors[primitive.indices];
        int count = accessor.count;

        std::vector<uint32_t> indices;
        indices.reserve(count);

        // Handle different component types
        switch (accessor.componentType)
        {
        case TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE:
        {
          const uint8_t *data = getData<uint8_t>(tinyModel, primitive.indices);
          for (int i = 0; i < count; ++i)
            indices.push_back(data[i]);
          break;
        }
        case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT:
        {
          const uint16_t *data = getData<uint16_t>(tinyModel, primitive.indices);
          for (int i = 0; i < count; ++i)
            indices.push_back(data[i]);
          break;
        }
        case TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT:
        {
          const uint32_t *data = getData<uint32_t>(tinyModel, primitive.indices);
          for (int i = 0; i < count; ++i)
            indices.push_back(data[i]);
          break;
        }
        default:
          std::cerr << "Unsupported index component type: " << accessor.componentType << std::endl;
          break;
        }

        // Add indices to mesh
        tmpmesh.indices = indices;
      }
      else
      {
        std::cout << "  - Warning: No indices found in primitive" << std::endl;
      }

      const tinygltf::Material &material = tinyModel.materials[primitive.material];
      const tinygltf::PbrMetallicRoughness &pbr = material.pbrMetallicRoughness;

      Vector3f baseCol;
      // If base color is too dark (sum of components < 0.1), use a default color

      baseCol = Vector3f(pbr.baseColorFactor[0], pbr.baseColorFactor[1], pbr.baseColorFactor[2]);

      tmpmesh.material = {
          .roughness = float(pbr.roughnessFactor),
          .metallicness = float(pbr.metallicFactor),
          .baseCol = baseCol,
          .baseTex = pbr.baseColorTexture.index,
          .metallicMap = pbr.metallicRoughnessTexture.index,
      };

      // Debug vertex positions and bounds
      Vector3f minBounds = Vector3f(std::numeric_limits<float>::max());
      Vector3f maxBounds = Vector3f(-std::numeric_limits<float>::max());

      tmpmesh.init();
      meshes.push_back(tmpmesh);
    }
  }
  return meshes;
}

std::vector<Texture> GLTFFile::getTextures()
{
  std::vector<Texture> textures;
  textures.resize(this->tinyModel.textures.size());

  for (size_t i = 0; i < this->tinyModel.textures.size(); i++)
  {
    const tinygltf::Texture &tex = this->tinyModel.textures[i];
    const tinygltf::Image &image = this->tinyModel.images[tex.source];

    // for debugging purposes
    /* for (int y = 0; y < image.height; y++)
    {
      for (int x = 0; x < image.width; x++)
      {

        std::cout << "(";
        for (int p = 0; p < 3; p++)
        {
          std::cout << (uint)image.image.data()[y * x * 3 + 3 * x + p] << ",";
        }
        std::cout << ") ";
      }
      std::cout << "\n";
    } */

    textures[tex.source] = Texture(int(image.width), int(image.height), (void *)image.image.data());
  }

  return textures;
}

std::vector<std::string> getJointNames(const tinygltf::Model &tinyModel)
{

  std::vector<std::string> names;
  names.resize(tinyModel.nodes.size());
  for (size_t i = 0; i < tinyModel.nodes.size(); i++)
  {
    const tinygltf::Node &node = tinyModel.nodes[i];
    names[i] = node.name;
  }

  return names;
}

Pose getRestPose(const tinygltf::Model &tinyModel)
{
  Pose result;
  result.resize(tinyModel.nodes.size());

  for (int i = 0; i < tinyModel.nodes.size(); i++)
  {

    const tinygltf::Node &node = tinyModel.nodes[i];

    Transform finalTransform;

    if (node.matrix.size() != 0)
    {
      const std::vector<double> &m = node.matrix;
      finalTransform = transformFromMat(
          Mat4x4(
              m[0], m[1], m[2], m[3],
              m[4], m[5], m[6], m[7],
              m[8], m[9], m[10], m[11],
              m[12], m[13], m[14], m[15])
              .transpose());
    }

    if (node.translation.size() != 0)
    {
      finalTransform.translation = Vector3f(node.translation[0], node.translation[1], node.translation[2]);
    }

    if (node.scale.size() != 0)
    {
      finalTransform.scaling = Vector3f(node.scale[0], node.scale[1], node.scale[2]);
    }

    if (node.rotation.size() != 0)
    {
      finalTransform.orientation = Quat(
          node.rotation[0],
          node.rotation[1],
          node.rotation[2],
          node.rotation[3]);
    }

    result.setLocalTransform(i, finalTransform);

    for (size_t j = 0; j < node.children.size(); j++)
    {
      result.setParent(node.children[j], i);
    }
  }

  return result;
}

std::vector<Mat4x4> getIverseMatrices(const tinygltf::Model &tinyModel)
{
  std::vector<Mat4x4> inverseMats;
  inverseMats.resize(tinyModel.nodes.size(), identity());

  const tinygltf::Skin &skin = tinyModel.skins[0];

  if (skin.inverseBindMatrices < 1)
  {
    std::cout << "no inverse bind matrices found!\n";
    return inverseMats;
  }
  const float *data = getData<float>(tinyModel, skin.inverseBindMatrices);

  for (size_t j = 0; j < skin.joints.size(); j++)
  {
    int index = skin.joints[j];
    inverseMats[index] = Mat4x4(&data[j * 16]).transpose();
  }
  return inverseMats;
}

Skeleton GLTFFile::getSkeleton()
{
  Skeleton result;

  result.jointNames = getJointNames(this->tinyModel);
  result.inversePose = getIverseMatrices(this->tinyModel);
  result.restPose = getRestPose(this->tinyModel);

  return result;
}

void editTrack(const tinygltf::Model &tinyModel,
               const tinygltf::AnimationSampler &animSampler,
               const tinygltf::AnimationChannel &channel,
               TransformTrack &track)
{

  const std::string path = channel.target_path;

  const float *timeData = getData<float>(tinyModel, animSampler.input);
  const float *valueData = getData<float>(tinyModel, animSampler.output);

  int count = tinyModel.accessors[animSampler.input].count;

  // std::cout << "channel target: " << channel.target_node << "\n";

  for (int j = 0; j < count; j++)
  {

    if (channel.target_path == "translation")
    {
      Frame<3> frame = {
          .m_value = {
              valueData[j * 3 + 0],
              valueData[j * 3 + 1],
              valueData[j * 3 + 2]},
          .m_in = {0.0f, 0.0f, 0.0f},
          .m_out = {0.0f, 0.0f, 0.0f},
          .time = timeData[j]};

      track.getPosTrack().frames.push_back(frame);
    }
    else if (channel.target_path == "rotation")
    {
      Frame<4> frame = {
          .m_value = {
              valueData[j * 4 + 0],
              valueData[j * 4 + 1],
              valueData[j * 4 + 2],
              valueData[j * 4 + 3]},

          .m_in = {0.0f, 0.0f, 0.0f, 0.0f},
          .m_out = {0.0f, 0.0f, 0.0f, 0.0f},
          .time = timeData[j]};
      track.getRotationTrack().frames.push_back(frame);
    }
    else if (channel.target_path == "scale")
    {
      Frame<3> frame = {
          .m_value = {
              valueData[j * 3 + 0],
              valueData[j * 3 + 1],
              valueData[j * 3 + 2]},
          .time = timeData[j]

      };
      track.getScalingTrack().frames.push_back(frame);
    }
  }
}

Clip getClip(const tinygltf::Model &tinyModel,
             const tinygltf::Animation &animation)
{
  Clip clip;

  for (size_t i = 0; i < animation.channels.size(); i++)
  {
    const tinygltf::AnimationChannel &channel = animation.channels[i];
    const tinygltf::AnimationSampler &animSampler = animation.samplers[channel.sampler];

    if (channel.target_node < 0)
      continue;

    bool exists = false;
    for (size_t joint = 0; joint < clip.size(); joint++)
    {
      if (clip.getTrack(joint).getId() == channel.target_node)
      {
        editTrack(tinyModel, animSampler, channel, clip.getTrack(joint));
        exists = true;
        break;
      }
    }

    if (!exists)
    {
      TransformTrack jointTrack;
      jointTrack.setId(channel.target_node);
      editTrack(tinyModel, animSampler, channel, jointTrack);
      clip.getTracks().push_back(jointTrack);
      clip.SetName(animation.name);
    }
  }

  // std::cout << "number of tracks: " << clip.size() << std::endl;
  clip.ReCalculateDuartion();
  return clip;
}

std::vector<Clip> GLTFFile::getClips()
{
  std::vector<Clip> clips;
  for (size_t i = 0; i < this->tinyModel.animations.size(); i++)
  {
    const tinygltf::Animation &animation = this->tinyModel.animations[i];
    clips.push_back(getClip(this->tinyModel, animation));
  }

  return clips;
}

template <typename T>
const T *getData(const tinygltf::Model &tinyModel, const int index)
{

  const tinygltf::Accessor &dataAccessor = tinyModel.accessors[index];
  const tinygltf::BufferView &dataBufferview = tinyModel.bufferViews[dataAccessor.bufferView];
  const tinygltf::Buffer &dataBuffer = tinyModel.buffers[dataBufferview.buffer];

  return reinterpret_cast<const T *>(&dataBuffer.data[dataBufferview.byteOffset + dataAccessor.byteOffset]);
}
