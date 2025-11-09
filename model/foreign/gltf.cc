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

  // Print basic model information
  std::cout << "Model loaded successfully:" << std::endl;
  std::cout << "- Number of meshes: " << this->tinyModel.meshes.size() << std::endl;
  std::cout << "- Number of materials: " << this->tinyModel.materials.size() << std::endl;
  std::cout << "- Number of textures: " << this->tinyModel.textures.size() << std::endl;
  std::cout << "- Number of nodes: " << this->tinyModel.nodes.size() << std::endl;
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

  std::cout << "\nProcessing meshes..." << std::endl;

  Mesh tmpmesh = {};
  tmpmesh.mode = TRIANGLES;

  for (size_t m = 0; m < this->tinyModel.meshes.size(); ++m)
  {
    tinygltf::Mesh &mesh = this->tinyModel.meshes[m];
    std::cout << "\nMesh[" << m << "] '" << mesh.name << "'" << std::endl;
    std::cout << "- Number of primitives: " << mesh.primitives.size() << std::endl;

    for (size_t j = 0; j < mesh.primitives.size(); ++j)
    {
      tmpmesh.vertices.clear();
      tmpmesh.indices.clear();

      tinygltf::Primitive &primitive = mesh.primitives[j];
      // positions
      auto it = primitive.attributes.find("POSITION");
      if (it != primitive.attributes.end())
      {
        const float *positions = getData<float>(this->tinyModel, it->second);
        int count = tinyModel.accessors[it->second].count;

        for (size_t i = 0; i < count; ++i)
        {
          Vertex vertex = {
              .pos = Vector3f(
                  positions[i * 3 + 0],
                  positions[i * 3 + 1],
                  positions[i * 3 + 2]),
          };
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
        const float *normals = getData<float>(this->tinyModel, it->second);
        int count = tinyModel.accessors[it->second].count;

        for (size_t i = 0; i < count; ++i)
        {

          tmpmesh.vertices[i].norm = Vector3f(
              normals[i * 3 + 0],
              normals[i * 3 + 1],
              normals[i * 3 + 2]);
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
        const float *uvs = getData<float>(this->tinyModel, it->second);
        int count = tinyModel.accessors[it->second].count;

        for (size_t i = 0; i < count; ++i)
        {

          tmpmesh.vertices[i].tc = Vector2f(
              uvs[i * 2 + 0],
              uvs[i * 2 + 1]);
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
        const unsigned short *joints =
            getData<unsigned short>(this->tinyModel, it->second);
        int count = tinyModel.accessors[it->second].count;

        std::vector<int> skinjoints;
        skinjoints = tinyModel.skins[0].joints;

        for (size_t i = 0; i < count; ++i)
        {

          tmpmesh.vertices[i].joints[0] = skinjoints[joints[i * 4 + 0]];
          tmpmesh.vertices[i].joints[1] = skinjoints[joints[i * 4 + 1]];
          tmpmesh.vertices[i].joints[2] = skinjoints[joints[i * 4 + 2]];
          tmpmesh.vertices[i].joints[3] = skinjoints[joints[i * 4 + 3]];
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
        const float *weights = getData<float>(this->tinyModel, it->second);
        int count = tinyModel.accessors[it->second].count;

        for (size_t i = 0; i < count; ++i)
        {
          tmpmesh.vertices[i].weights[0] = weights[i * 4 + 0];
          tmpmesh.vertices[i].weights[1] = weights[i * 4 + 1];
          tmpmesh.vertices[i].weights[2] = weights[i * 4 + 2];
          tmpmesh.vertices[i].weights[3] = weights[i * 4 + 3];
        };
      }
      else
      {
        std::cout << "no weights attributes found in primitive " << j
                  << " of mesh " << m << "\n";
      }

      if (primitive.indices >= 0)
      {
        const uint *indices = getData<uint>(tinyModel, primitive.indices);
        int count = tinyModel.accessors[primitive.indices].count;
        std::cout << "  - Index count: " << count << std::endl;
        for (int i = 0; i < count; ++i)
        {
          tmpmesh.indices.push_back(indices[i]);
        }
      }
      else
      {
        std::cout << "  - Warning: No indices found in primitive" << std::endl;
      }
      
      std::cout << "  - Vertex count: " << tmpmesh.vertices.size() << std::endl;
      std::cout << "  - Index count: " << tmpmesh.indices.size() << std::endl;

      const tinygltf::Material &material =
          tinyModel.materials[primitive.material];
      const tinygltf::PbrMetallicRoughness &pbr = material.pbrMetallicRoughness;

      Vector3f baseCol;
      // If base color is too dark (sum of components < 0.1), use a default color
      if (pbr.baseColorFactor[0] + pbr.baseColorFactor[1] + pbr.baseColorFactor[2] < 0.1) {
          std::cout << "Warning: Very dark material detected, using default color" << std::endl;
          baseCol = Vector3f(0.7f, 0.7f, 0.7f);  // Default to light gray
      } else {
          baseCol = Vector3f(pbr.baseColorFactor[0], pbr.baseColorFactor[1],
                           pbr.baseColorFactor[2]);
      }

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
      
      std::cout << "\nMesh bounds check:" << std::endl;
      for (const auto& vertex : tmpmesh.vertices) {
          minBounds.x = std::min(minBounds.x, vertex.pos.x);
          minBounds.y = std::min(minBounds.y, vertex.pos.y);
          minBounds.z = std::min(minBounds.z, vertex.pos.z);
          
          maxBounds.x = std::max(maxBounds.x, vertex.pos.x);
          maxBounds.y = std::max(maxBounds.y, vertex.pos.y);
          maxBounds.z = std::max(maxBounds.z, vertex.pos.z);
      }
      
      std::cout << "- Bounds min: (" << minBounds.x << ", " << minBounds.y << ", " << minBounds.z << ")" << std::endl;
      std::cout << "- Bounds max: (" << maxBounds.x << ", " << maxBounds.y << ", " << maxBounds.z << ")" << std::endl;
      std::cout << "- Material properties:" << std::endl;
      std::cout << "  * Base color: (" << baseCol.x << ", " << baseCol.y << ", " << baseCol.z << ")" << std::endl;
      std::cout << "  * Roughness: " << pbr.roughnessFactor << std::endl;
      std::cout << "  * Metallic: " << pbr.metallicFactor << std::endl;
      std::cout << "  * Base texture index: " << pbr.baseColorTexture.index << std::endl;

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

    textures[tex.source] = Texture(int(image.width), int(image.height),
                                   (void *)image.image.data());
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
      finalTransform.translation = Vector3f(
          node.translation[0], node.translation[1], node.translation[2]);
    }

    if (node.scale.size() != 0)
    {
      finalTransform.scaling =
          Vector3f(node.scale[0], node.scale[1], node.scale[2]);
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
    const tinygltf::AnimationSampler &animSampler =
        animation.samplers[channel.sampler];

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
  const tinygltf::BufferView &dataBufferview =
      tinyModel.bufferViews[dataAccessor.bufferView];
  const tinygltf::Buffer &dataBuffer = tinyModel.buffers[dataBufferview.buffer];

  return reinterpret_cast<const T *>(
      &dataBuffer.data[dataBufferview.byteOffset + dataAccessor.byteOffset]);
}
