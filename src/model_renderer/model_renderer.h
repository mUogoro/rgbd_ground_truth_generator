#ifndef __MODEL_RENDERER_H
#define __MODEL_RENDERER_H

/*! \file model_generator.h
 * \brief C wrapper for the ModelRenderer C++ class.
 *
 */

#ifdef __cplusplus
extern "C" {
#endif

/*! \struct Opaque ModelRenderer structure
 *
 */
typedef struct ModelRenderer ModelRenderer;


/*! \brief Return a new ModelRenderer structure instance
 * 
 * \return A new ModelRenderer structure distance.
 *
 */
ModelRenderer *ModelRenderer_new();


/*! \brief Initialize a ModelRenderer instance with default parameters
 * The default parameters are output width,height = [640,480], default 
 * horizontal,vertical FOV = [57,43](degrees), near,far clipping planes = [30,80](cm)
 *
 * \param self A ModelRenderer struct previously instantiated using ModelRenderer_new
 *
 * \note: The model.dae file containing the model mesh/skeleton must be saved in the current
 *        working directory
 *
 *
 */
void ModelRenderer_initDefault(ModelRenderer *self);


/*! \brief Initialize a ModelRenderer instance
 * 
 * \param self A ModelRenderer struct previously instantiated using ModelRenderer_new
 * \param width Output X resolution
 * \param height Output Y resolution
 * \param modelName Path of the model (typically a collada file)
 * \param hFOV Horizontal Field Of View (degrees)
 * \param vFOV Vertical Field Of View (degrees)
 * \param near Near clipping plane (cm)
 * \param far Far clipping plane (cm)
 *
 */
void ModelRenderer_init(ModelRenderer *self, unsigned int width, unsigned int height,
			const char *modelName,
			float hFOV, float vFOV, float near, float far);

/*! \brief Return rendering width
 *
 *  \param self A ModelRenderer struct previously instantiated using ModelRenderer_new
 *  \return Rendering width
 */  
int ModelRenderer_getWidth(ModelRenderer *self);

/*! \brief Return rendering height
 *
 *  \param self A ModelRenderer struct previously instantiated using ModelRenderer_new
 *  \return Rendering height
 */  
int ModelRenderer_getHeight(ModelRenderer *self);

/*! \brief Return rendering near clipping distance
 *
 *  \param self A ModelRenderer struct previously instantiated using ModelRenderer_new
 *  \return Rendering near clipping distance
 */  
float ModelRenderer_getNear(ModelRenderer *self);

/*! \brief Return rendering far clipping distance
 *
 *  \param self A ModelRenderer struct previously instantiated using ModelRenderer_new
 *  \return Rendering far clipping distance
 */  
float ModelRenderer_getFar(ModelRenderer *self);

/*! \brief Set the current model skeleton pose
 * Animate the model skeleton using the specified pose and deform the model mesh accordingly.
 * The pose is specified as a vector of 4x4 matrices defined in column-major order. Each
 * matrix define an affine transformation with respect to the bone's local frame of reference.
 *
 * \param self A ModelRenderer struct previously instantiated using ModelRenderer_new
 * \param transformation Vector of 4x4 float matrices defining the local transformation at
 *        each bone
 *
 */
void ModelRenderer_setPose(ModelRenderer *self, float *transformation);

/*! \brief Perform the depthmap rendering
 * The depthmap is rendered a 16bit precision grayscale image, where each pixel stores the
 * distance (in mm) from the sensor with respect to the XY camera plane.
 *
 * \param self A ModelRenderer struct previously instantiated using ModelRenderer_new
 * \param color Buffer where the rendering is stored
 * \param depth Buffer where the depthmap is stored
 *
 */
void ModelRenderer_render(ModelRenderer *self, unsigned char *color, unsigned short *depth);

/*! \brief Get the number of skeleton bones
 * 
 * \param self A ModelRenderer struct previously instantiated using ModelRenderer_new
 * \return Number of model skeleton bones
 *
 */
unsigned int ModelRenderer_getBonesNum(ModelRenderer *self);

/*! \brief Set bones length
 *
 * \param self A ModelRenderer struct previously instantiated using ModelRenderer_new
 * \param bonesLen Bones length
 *
 */
void ModelRenderer_setBonesLen(ModelRenderer *self, float *bonesLen);

/*! \brief Get the name of idx-th bone
 * 
 * \param self A ModelRenderer struct previously instantiated using ModelRenderer_new
 * \param idx Unique bone's identifier
 * \return The unique ID of the bone with name "name"
 *
 */
int ModelRenderer_getBoneIdx(ModelRenderer *self, const char *name);

/*! \brief Get the unique id of bone with name "name"
 * 
 * \param self A ModelRenderer struct previously instantiated using ModelRenderer_new
 * \param name Name of the bone if found, -1 otherwise
 *
 */
const char *ModelRenderer_getBoneName(ModelRenderer *self, unsigned int idx);

/*! \brief Get bone head
 * The bone head represents the 3D position of the bone's reference frame origin.
 * Bone rotations happens around this point. Bone's head coordinates are stored as
 * homogeneous coordinates.
 *
 * \param self A ModelRenderer struct previously instantiated using ModelRenderer_new
 * \param idx Unique bone's identifier
 * \return Coordinates of bone's head as a 4-element array
 *
 */
const float *ModelRenderer_getBoneHead(ModelRenderer *self, unsigned int idx);

/*! \brief Get bone tail
 * The bone tail represents the bone's end-point. The tail is placed along the bone's
 * up-vector (i.e. local y axis). Bone's tail coordinates are stored as homogeneous
 * coordinates.
 *
 * \param self A ModelRenderer struct previously instantiated using ModelRenderer_new
 * \param idx Unique bone's identifier
 * \return Coordinates of bone's tail as a 4-element array
 *
 */
const float *ModelRenderer_getBoneTail(ModelRenderer *self, unsigned int idx);

/*! \brief Get bone frame of reference (world coordinates)
 * 
 * \param self A ModelRenderer struct previously instantiated using ModelRenderer_new
 * \param idx Unique bone's identifier
 * \return The bones frame of reference as a 4x4 matrix in homogeneous coordinates,
 * stored in column-major order
 *
 */
const float *ModelRenderer_getBoneBase(ModelRenderer *self, unsigned int idx);

/*! \brief Clear resources allocated for current ModelRenderer instance
 * 
 * \param self A ModelRenderer struct previously instantiated using ModelRenderer_new
 *
 */
void ModelRenderer_delete(ModelRenderer *self);

#ifdef __cplusplus
}
#endif

#endif /* __MODEL_RENDERER_H */
