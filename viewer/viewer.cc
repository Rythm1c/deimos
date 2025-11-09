#include "viewer.h"
#include "../model/model.h"

Viewer::Viewer()
    : camera(new Camera()),
      currModel("None"),
      lightDir(Vector3f(0.5, -0.5, 0.5)),
      phongStatic(nullptr),
      phongAnimated(nullptr),
      pbrStatic(nullptr),
      pbrAnimated(nullptr) {}

Viewer::~Viewer()
{
  delete this->camera;

  if (this->pbrAnimated != nullptr)
  {
    this->pbrAnimated->clean();
    delete this->pbrAnimated;
  }
  if (this->pbrStatic != nullptr)
  {
    this->pbrStatic->clean();
    delete this->pbrStatic;
  }
  if (this->phongStatic != nullptr)
  {
    this->phongStatic->clean();
    delete this->phongStatic;
  }
  if (this->phongAnimated != nullptr)
  {
    this->phongAnimated->clean();
    delete this->phongAnimated;
  }

  for (auto &model : models)
  {
    model.second->clean();
    delete model.second;
  }
}
Model *Viewer::getCurrModel()
{
  if (this->currModel != "None")
  {
    return this->models[this->currModel];
  }
  return nullptr;
}

void Viewer::init()
{
  this->phongStatic = new Shader("shaders/shader.vert", "shaders/shader.frag");
  this->phongAnimated =
      new Shader("shaders/animation.vert", "shaders/shader.frag");
  this->pbrStatic = new Shader("shaders/shader.vert", "shaders/pbr.frag");
  this->pbrAnimated = new Shader("shaders/animation.vert", "shaders/pbr.frag");

  // Initialize debug renderer
  this->debugRenderer.init();

  this->phongStatic->updateInt("baseTex", 0);
  this->phongStatic->updateInt("metallicMap", 1);
  this->phongStatic->updateInt("normalMap", 2);

  this->phongAnimated->updateInt("albedoMap", 0);
  this->phongAnimated->updateInt("metallicMap", 1);
  this->phongAnimated->updateInt("normalMap", 2);

  this->pbrStatic->updateInt("albedoMap", 0);
  this->pbrStatic->updateInt("metallicMap", 1);
  this->pbrStatic->updateInt("normalMap", 2);

  this->pbrAnimated->updateInt("albedoMap", 0);
  this->pbrAnimated->updateInt("metallicMap", 1);
  this->pbrAnimated->updateInt("normalMap", 2);

  this->lights.push_back(
      {.color = {300.0, 0.0, 300.0}, .position = {60.0, 10.0, -60.0}});
  this->lights.push_back(
      {.color = {300.0, 300.0, 300.0}, .position = {60.0, 10.0, 60.0}});
  this->lights.push_back(
      {.color = {300.0, 300.0, 0.0}, .position = {-60.0, 10.0, 60.0}});
  this->lights.push_back(
      {.color = {0.0, 300.0, 300.0}, .position = {-60.0, 10.0, -60.0}});
}

void Viewer::addModel(std::string name, std::string path)
{
  try
  {
    std::cout << "\nAdding model: " << name << " from path: " << path << std::endl;

    Model *model = new Model();
    GLTFFile file = GLTFFile(path);
    file.populateModel(*model);

    // Validate model data
    if (model->meshes.empty())
    {
      std::cerr << "Warning: Model has no meshes!" << std::endl;
    }
    else
    {
      std::cout << "Model loaded successfully with " << model->meshes.size() << " meshes" << std::endl;

      // Print mesh information
      for (size_t i = 0; i < model->meshes.size(); i++)
      {
        std::cout << "Mesh " << i << ":" << std::endl;
        std::cout << "- Vertices: " << model->meshes[i].vertices.size() << std::endl;
        std::cout << "- Indices: " << model->meshes[i].indices.size() << std::endl;
      }
    }

    model->scale(Vector3f(2.5));
    model->orient(Quat(180.0, Vector3f(0.0, 1.0, 0.0)));
    model->translate(Vector3f(0.0, 0.0, 5.0));

    model->animController->setCurrentAnimation(0);
    model->animController->play();
    this->models.insert(std::make_pair(name, model));

    std::cout << "Model added successfully" << std::endl;
  }
  catch (const std::exception &e)
  {
    std::cerr << "Error adding model: " << e.what() << std::endl;
    throw;
  }
}

std::vector<std::string> Viewer::getModelNames()
{
  std::vector<std::string> names;
  for (const auto &pair : this->models)
  {
    names.push_back(pair.first);
  }
  return names;
}

void Viewer::update(float ratio, float delta)
{

  this->phongStatic->use();
  this->phongStatic->updateVec3("lightDirection", this->lightDir);
  this->phongStatic->updateVec3("viewPos", this->camera->pos);
  this->phongStatic->updateMat4("view", this->camera->view());
  this->phongStatic->updateMat4("projection", this->camera->projection(ratio));

  this->phongAnimated->use();
  this->phongAnimated->updateVec3("lightDirection", this->lightDir);
  this->phongAnimated->updateVec3("camPos", this->camera->pos);
  this->phongAnimated->updateMat4("view", this->camera->view());
  this->phongAnimated->updateMat4("projection",
                                  this->camera->projection(ratio));

  this->pbrAnimated->use();
  this->pbrAnimated->updateVec3("lightDirection", this->lightDir);
  this->pbrAnimated->updateVec3("camPos", this->camera->pos);
  this->pbrAnimated->updateMat4("view", this->camera->view());
  this->pbrAnimated->updateMat4("projection", this->camera->projection(ratio));

  this->models[this->currModel]->animController->update(delta);
}

void Viewer::renderCurrModel()
{
  /* this->phongStatic->use();
  this->phongStatic->updateInt("textured", false);
  if (this->currModel != "None")
  {
    this->phongStatic->updateVec3("inColor",
  this->models[this->currModel]->color);
    this->phongStatic->updateMat4("transform",
  this->models[this->currModel]->get_transform());
    this->models[this->currModel]->render();
  } */

  /*this->phongAnimated->use();
  this->phongAnimated->updateInt("textured", false);
  if (this->currModel != "None") {

    this->phongAnimated->updateMat4(
        "transform", this->models[this->currModel]->get_transform());

    std::vector<Mat4x4> mats = this->models[this->currModel]->getPose();
    for (int i = 0; i < mats.size(); i++) {
      std::string value = "boneMats[" + std::to_string(i) + "]";
      this->phongAnimated->updateMat4(value.c_str(), mats[i]);
    }
    this->models[this->currModel]->render(*this->phongAnimated);
  }8*/

  this->pbrAnimated->use();
  this->pbrAnimated->updateInt("textured", false);
  if (this->currModel != "None")
  {

    for (size_t i = 0; i < this->lights.size(); ++i)
    {

      const Light &light = this->lights[i];
      this->pbrAnimated->updateInt("lightCount", int(this->lights.size()));
      std::string value = "lights[" + std::to_string(i) + "]";
      this->pbrAnimated->updateVec3((value + ".color").c_str(), light.color);
      this->pbrAnimated->updateVec3((value + ".position").c_str(),
                                    light.position);
    }

    this->pbrAnimated->updateMat4(
        "transform", this->models[this->currModel]->get_transform());

    std::vector<Mat4x4> mats = this->models[this->currModel]->animController->getPose();
    for (size_t i = 0; i < mats.size(); i++)
    {
      std::string value = "boneMats[" + std::to_string(i) + "]";
      this->pbrAnimated->updateMat4(value.c_str(), mats[i]);
    }
    this->models[this->currModel]->render(*this->pbrAnimated);

    // Render bounding boxes if enabled
    /* if (showBoundingBoxes) {
      Mat4x4 modelTransform = this->models[this->currModel]->get_transform();
      Mat4x4 viewMat = this->camera->view();
      Mat4x4 projMat = this->camera->projection(16.0f/9.0f);  // Use appropriate aspect ratio

      // Render bounding box for each mesh
      for (const auto& mesh : this->models[this->currModel]->meshes) {
        BoundingBox bbox = mesh.getBoundingBox();
        this->debugRenderer.renderBoundingBox(bbox, modelTransform, viewMat, projMat);
      }
    } */
  }
}
