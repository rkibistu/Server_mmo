#include "Frustrum.h"
#include "core/SceneManager.h"
#include "core/AABB.h"

#include "core/MemoryDebug.h"

Frustrum::Frustrum() {
}

bool Frustrum::IsInFrustrum(AABB& aabb) {
	// For each plane, check if all corners of the AABB are in front of the plane
	rml::Vector3 corners[8] = {
		rml::Vector3(aabb.Min.x, aabb.Min.y, aabb.Min.z),
		rml::Vector3(aabb.Max.x, aabb.Min.y, aabb.Min.z),
		rml::Vector3(aabb.Min.x, aabb.Max.y, aabb.Min.z),
		rml::Vector3(aabb.Max.x, aabb.Max.y, aabb.Min.z),
		rml::Vector3(aabb.Min.x, aabb.Min.y, aabb.Max.z),
		rml::Vector3(aabb.Max.x, aabb.Min.y, aabb.Max.z),
		rml::Vector3(aabb.Min.x, aabb.Max.y, aabb.Max.z),
		rml::Vector3(aabb.Max.x, aabb.Max.y, aabb.Max.z)
	};


	//Simple chacking the corners doesn t detect the case when furstrum intersects an edge,
	// so we check that all corners are or not to the outside part of the same plane
	// check ehre for details: https://cgvr.cs.uni-bremen.de/teaching/cg_literatur/lighthouse3d_view_frustum_culling/index.html
	for (auto& plane : _planes) {

		bool atLeastOneIn = false, atLeastOneOut = false;
		for (int i = 0; i < 8; i++) {
			if (plane.second.distanceToPoint(corners[i]) < 0) {
				atLeastOneOut = true;
			}
			else {
				atLeastOneIn = true;
				break;
			}
			
			if (atLeastOneIn && atLeastOneOut)
				break;
		}
		if (!atLeastOneIn)
			return false; 
	}
	return true;
}

bool Frustrum::IsInFrustrum(rml::Vector3 point) {
	for (auto& plane : _planes) {
		if (plane.second.distanceToPoint(point) < 0)
			return false;
	}
	return true;
}

void Frustrum::GenerateCorners(Camera* camera) {

	ProjectionInfo projInfo = camera->GetProjectionInfo();
	rml::Vector3 cameraPos = camera->GetPosition();
	rml::Vector3 cameraForward = camera->GetForward();
	rml::Vector3 cameraUp = camera->GetUp();
	rml::Vector3 cameraRight = camera->GetRight();

	float heightNear = 2 * tan(projInfo.FoV / 2) * projInfo.Znear;
	float widthNear = heightNear * projInfo.AspectRatio;

	float heightFar = 2 * tan(projInfo.FoV / 2) * projInfo.Zfar;
	float widthFar = heightFar * projInfo.AspectRatio;

	rml::Vector3 fc = cameraPos + cameraForward * projInfo.Zfar;
	rml::Vector3 nc = cameraPos + cameraForward * projInfo.Znear;

	_ftl = fc + (cameraUp * heightFar / 2) - (cameraRight * widthFar / 2);
	_ftr = fc + (cameraUp * heightFar / 2) + (cameraRight * widthFar / 2);
	_fbl = fc - (cameraUp * heightFar / 2) - (cameraRight * widthFar / 2);
	_fbr = fc - (cameraUp * heightFar / 2) + (cameraRight * widthFar / 2);

	_ntl = nc + (cameraUp * heightNear / 2) - (cameraRight * widthNear / 2);
	_ntr = nc + (cameraUp * heightNear / 2) + (cameraRight * widthNear / 2);
	_nbl = nc - (cameraUp * heightNear / 2) - (cameraRight * widthNear / 2);
	_nbr = nc - (cameraUp * heightNear / 2) + (cameraRight * widthNear / 2);

	// Generate plane
	Plane plane;
	// Near plane
	plane.normal = cameraForward;
	plane.normal.Normalize();
	plane.point = nc;
	_planes["NEAR"] = plane;
	// Far plane
	plane.normal = -cameraForward;
	plane.normal.Normalize();
	plane.point = fc;
	_planes["FAR"] = plane;
	// Top plane
	rml::Vector3 aux;
	aux = (nc + cameraUp * heightNear / 2) - cameraPos;
	aux.Normalize();
	plane.normal = cameraRight.Cross(aux);
	plane.normal.Normalize();
	plane.point = nc + cameraUp * heightNear / 2;
	_planes["TOP"] = plane;
	// Bottom p[lane
	aux = (nc - cameraUp * heightNear / 2) - cameraPos;
	aux.Normalize();
	plane.normal = aux.Cross(cameraRight);
	plane.normal.Normalize();
	plane.point = nc - cameraUp * heightNear / 2;
	_planes["BOTTOM"] = plane;
	// Left plane
	aux = (nc - cameraRight * widthNear / 2) - cameraPos;
	aux.Normalize();
	plane.normal = cameraUp.Cross(aux);
	plane.normal.Normalize();
	plane.point = nc - cameraRight * widthNear / 2;
	_planes["LEFT"] = plane;
	// Right plane
	aux = (nc + cameraRight * widthNear / 2) - cameraPos;
	aux.Normalize();
	plane.normal = aux.Cross(cameraUp);
	plane.normal.Normalize();
	plane.point = nc + cameraRight * widthNear / 2;
	_planes["RIGHT"] = plane;
}

void Frustrum::Draw() {

	//Draw far plane
	Renderer::DrawLine(_ftl, _ftr, rml::Vector3(1, 0, 0));
	Renderer::DrawLine(_ftr, _fbr, rml::Vector3(1, 0, 0));
	Renderer::DrawLine(_fbr, _fbl, rml::Vector3(1, 0, 0));
	Renderer::DrawLine(_fbl, _ftl, rml::Vector3(1, 0, 0));

	Renderer::DrawLine(_ftl, _fbl, rml::Vector3(1, 0, 0));
	Renderer::DrawLine(_fbl, _nbl, rml::Vector3(1, 0, 0));
	Renderer::DrawLine(_nbl, _ntl, rml::Vector3(1, 0, 0));
	Renderer::DrawLine(_ntl, _ftl, rml::Vector3(1, 0, 0));

	Renderer::DrawLine(_ftr, _fbr, rml::Vector3(1, 0, 0));
	Renderer::DrawLine(_fbr, _nbr, rml::Vector3(1, 0, 0));
	Renderer::DrawLine(_nbr, _ntr, rml::Vector3(1, 0, 0));
	Renderer::DrawLine(_ntr, _ftr, rml::Vector3(1, 0, 0));

	//Draw plane normals
	float length = 0.2;
	Renderer::DrawLine(_planes["TOP"].point,	_planes["TOP"].point +		_planes["TOP"].normal * length,		rml::Vector3(1, 1, 0));
	Renderer::DrawLine(_planes["BOTTOM"].point, _planes["BOTTOM"].point +	_planes["BOTTOM"].normal * length,	rml::Vector3(1, 1, 0));
	Renderer::DrawLine(_planes["LEFT"].point,	_planes["LEFT"].point +		_planes["LEFT"].normal * length,	rml::Vector3(1, 1, 0));
	Renderer::DrawLine(_planes["RIGHT"].point,	_planes["RIGHT"].point +	_planes["RIGHT"].normal * length,	rml::Vector3(1, 1, 0));
	Renderer::DrawLine(_planes["NEAR"].point,	_planes["NEAR"].point +		_planes["NEAR"].normal * length,	rml::Vector3(1, 1, 0));
	Renderer::DrawLine(_planes["FAR"].point,	_planes["FAR"].point +		_planes["FAR"].normal * length,		rml::Vector3(1, 1, 0));
}
