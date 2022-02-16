#include "PerspectiveCamera.hpp"

namespace Haku
{
namespace Renderer
{
PerspectiveCamera::PerspectiveCamera(float Aspect_ratio, float FOV, float NearZ, float FarZ)
	: m_FOV(FOV)
	, m_FarZ(FarZ)
	, m_NearZ(NearZ)
	, m_Aspect_ratio(Aspect_ratio)
{
	m_Camera = DirectX::XMMatrixPerspectiveFovLH(FOV, Aspect_ratio, NearZ, FarZ);
}
void PerspectiveCamera::SetFarZ(float& Z) noexcept
{
	m_FarZ	 = Z;
	m_Camera = DirectX::XMMatrixPerspectiveFovLH(m_FOV, m_Aspect_ratio, m_NearZ, m_FarZ);
}
void PerspectiveCamera::SetNearZ(float& Z) noexcept
{
	m_NearZ	 = Z;
	m_Camera = DirectX::XMMatrixPerspectiveFovLH(m_FOV, m_Aspect_ratio, m_NearZ, m_FarZ);
}
void PerspectiveCamera::SetFOV(float& FOV) noexcept
{
	m_FOV	 = FOV;
	m_Camera = DirectX::XMMatrixPerspectiveFovLH(m_FOV, m_Aspect_ratio, m_NearZ, m_FarZ);
}
void PerspectiveCamera::SetAspectRatio(float& ratio) noexcept
{
	m_Aspect_ratio = ratio;
	m_Camera	   = DirectX::XMMatrixPerspectiveFovLH(m_FOV, m_Aspect_ratio, m_NearZ, m_FarZ);
}
void PerspectiveCamera::RotateCamera(float ptr[3]) noexcept
{
	m_Rotation = DirectX::XMMatrixTranspose(
		DirectX::XMMatrixRotationX(ptr[0]) * DirectX::XMMatrixRotationY(ptr[1]) * DirectX::XMMatrixRotationZ(ptr[2]));
}

void PerspectiveCamera::RotateCamera(DirectX::XMMATRIX& ref) noexcept
{
	m_Rotation = ref;
}
void PerspectiveCamera::TranslateCamera(float ptr[3]) noexcept
{
	m_Translation = DirectX::XMMatrixTranslation(ptr[0], ptr[1], ptr[2]);
}

void PerspectiveCamera::TranslateCamera(DirectX::XMMATRIX& ref) noexcept
{
	m_Translation = ref;
}

void PerspectiveCamera::SetCameraParameter(Camera_parameter parameter) noexcept
{
	switch (parameter.type)
	{
	case CAM_PARAMETER::CAMERA_FOV:
	{
		SetFOV(parameter.parameter);
		break;
	};
	case CAM_PARAMETER::CAMERA_FARZ:
	{
		SetFarZ(parameter.parameter);
		break;
	};
	case CAM_PARAMETER::CAMERA_NEARZ:
	{
		SetNearZ(parameter.parameter);
		break;
	};
	case CAM_PARAMETER::CAMERA_ASPECT_RATIO:
	{
		SetAspectRatio(parameter.parameter);
		break;
	};
	}
}

} // namespace Renderer
} // namespace Haku
