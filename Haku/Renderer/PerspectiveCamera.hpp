#include "../hakupch.hpp"
#include "DirectXMath.h"

// NOTES : the translation matrix is multiplied with -1(for the obvious reflection behaviour seen)
// NOTES : the FOV is in taken in and stored in degrees and calculated to radians for matrix
// TODO  : check and seen if the transformations can be extracted in a sensible manner
// void TakeTransform(DirectX::XMMATRIX& ref) noexcept;

namespace Haku
{
namespace Renderer
{
enum CAM_PARAMETER
{
	CAMERA_FOV = 0,
	CAMERA_FARZ,
	CAMERA_NEARZ,
	CAMERA_ASPECT_RATIO
};
struct Camera_parameter
{
	CAM_PARAMETER type;
	float		  parameter;
};
class PerspectiveCamera
{
public:
	PerspectiveCamera(float Aspect_ratio = 0.75, float FOV = 60.0f, float NearZ = 0.01f, float FarZ = 100.0f);
	void SetFarZ(float Z) noexcept;
	void SetNearZ(float Z) noexcept;
	void SetFOV(float FOV) noexcept;
	void SetAspectRatio(float ratio) noexcept;

	// general rotation functions
	void RotateCamera(float ptr[3]) noexcept;
	void RotateCamera(DirectX::XMMATRIX& ref) noexcept;

	// general translation functions
	void TranslateCamera(float ptr[3]) noexcept;
	void TranslateCamera(DirectX::XMMATRIX& ref) noexcept;

	void SetCameraParameter(struct Camera_parameter parameter) noexcept;
	// Transformation matrix pass through for precalculated 4x4 matrix
	// might just take this out so as to not worry about extracting individual transformation matrix
	// void TakeTransform(DirectX::XMMATRIX& ref) noexcept;

	DirectX::XMMATRIX GetCamera() { return DirectX::XMMatrixTranspose(m_Camera); }

private:
	float			  m_FOV;
	float			  m_FarZ;
	float			  m_NearZ;
	DirectX::XMMATRIX m_Camera;
	DirectX::XMMATRIX m_Rotation;
	DirectX::XMMATRIX m_Translation;
	float			  m_Aspect_ratio;
};
} // namespace Renderer
} // namespace Haku
