#include <iostream>
#include <string>

#include "Camera.h"
#include <core/Engine.h>
#include <core/culling/Frustrum.h>

#include <core/MemoryDebug.h>

Camera::Camera(unsigned int id, std::string name)
	: _id(id), _name(name) {

	if (name == "") {
		_name = "Camera_" + std::to_string(id);
	}
}

Camera::Camera(unsigned int id, rml::Vector3 position, rml::Vector3 target, rml::Vector3 up, std::string name)
	:Camera(id, name) {

	Init(position, target, up, 45.0f, 0.1f, 2000.0f);
}

Camera::~Camera() {
	if (_frustrum) {
		delete _frustrum;
		_frustrum = nullptr;
	}
}

void Camera::Init(rml::Vector3 position, rml::Vector3 target, rml::Vector3 up, float fov, float zNear, float zFar) {

	_position = position;
	_target = target;
	_up = up;

	_projectionInfo.FoV = fov;
	_projectionInfo.Znear = zNear;
	_projectionInfo.Zfar = zFar;
	_projectionInfo.Width = Engine::GetWindow()->GetResolution().x;
	_projectionInfo.Height = Engine::GetWindow()->GetResolution().y;
	_projectionInfo.AspectRatio = _projectionInfo.Width / _projectionInfo.Height;
	_projectionInfo.IsPerspective = true;

	_projMatrix.SetPerspective(_projectionInfo.FoV, _projectionInfo.AspectRatio, _projectionInfo.Znear, _projectionInfo.Zfar);

	_frustrum = new Frustrum();

	UpdateAxis();
	UpdateWorldView();
}

void Camera::SetPerspective(float fov, float widht, float height, float zNear, float zFar) {
	_projectionInfo.FoV = fov;
	_projectionInfo.Znear = zNear;
	_projectionInfo.Zfar = zFar;
	_projectionInfo.Width = widht;
	_projectionInfo.Height = height;
	_projectionInfo.AspectRatio = _projectionInfo.Width / _projectionInfo.Height;;
	_projectionInfo.IsPerspective = true;

	_projMatrix.SetPerspective(_projectionInfo.FoV, _projectionInfo.AspectRatio, _projectionInfo.Znear, _projectionInfo.Zfar);
}

void Camera::MoveForward(float deltaTime) {
	rml::Vector3 velocity = GetForward() * _moveSpeed * deltaTime;

	_position += velocity;
	_target += velocity;

	UpdateWorldView();
}

void Camera::MoveBackward(float deltaTime) {
	rml::Vector3 velocity = -GetForward() * _moveSpeed * deltaTime;

	_position += velocity;
	_target += velocity;

	UpdateWorldView();
}

void Camera::MoveRight(float deltaTime) {
	rml::Vector3 velocity = GetRight() * _moveSpeed * deltaTime;

	_position += velocity;
	_target += velocity;

	UpdateWorldView();
}

void Camera::MoveLeft(float deltaTime) {
	rml::Vector3 velocity = -GetRight() * _moveSpeed * deltaTime;

	_position += velocity;
	_target += velocity;

	UpdateWorldView();
}

void Camera::MoveUp(float deltaTime) {
	rml::Vector3 velocity = GetUp() * _moveSpeed * deltaTime;

	_position += velocity;
	_target += velocity;

	UpdateWorldView();
}

void Camera::MoveDown(float deltaTime) {
	rml::Vector3 velocity = -GetUp() * _moveSpeed * deltaTime;

	_position += velocity;
	_target += velocity;

	UpdateWorldView();
}

void Camera::RotateOX(float deltaTime) {
	// go to center -> for rotation to be correct
	rml::Vector3 goBackPos(_position);
	TranslateToOrigin();

	rml::Matrix rotateOX;
	rotateOX.SetRotationX(_rotationSpeedVertical * deltaTime);

	_rotation.x += _rotationSpeedVertical * deltaTime;

	rml::Vector4 rotateLocalUp = rml::Vector4(_localUp, 1.0f) * rotateOX;
	rml::Vector4 up = (rotateLocalUp * _worldMatrix).Normalize();
	_up = rml::Vector3(up.x, up.y, up.z);

	rml::Vector4 localTarget(0.0f, 0.0f, -(_target - _position).Length(), 1.0f);
	rml::Vector4 rotatedTarget = localTarget * rotateOX;
	rml::Vector4 target = rotatedTarget * _worldMatrix;
	_target = rml::Vector3(target.x, target.y, target.z);

	UpdateWorldView();

	TranslateToPosition(goBackPos);
}

void Camera::RotateOY(float deltaTime) {
	// go to center -> for rotation to be correct
	rml::Vector3 goBackPos(_position);
	TranslateToOrigin();

	rml::Vector4 localTarget(0.0f, 0.0f, -(_target - _position).Length(), 1.0f);

	rml::Matrix rotateOY;
	rotateOY.SetRotationY(_rotationSpeedHorizontal * deltaTime);
	_rotation.y += _rotationSpeedHorizontal * deltaTime;

	rml::Vector4 rotatedTarget = localTarget * rotateOY;

	rml::Vector4 temp = rotatedTarget * _worldMatrix;
	_target.x = temp.x;
	_target.y = temp.y;
	_target.z = temp.z;

	UpdateWorldView();

	// go back to initial pos before starting rotation
	TranslateToPosition(goBackPos);
}

void Camera::RotateOZ(float deltaTime) {
	// go to center -> for rotation to be correct
	rml::Vector3 goBackPos(_position);
	TranslateToOrigin();

	rml::Matrix rotateOZ;
	rotateOZ.SetRotationZ(_rotationSpeedHorizontal * deltaTime);
	_rotation.z += _rotationSpeedHorizontal * deltaTime;

	rml::Vector4 rotateLocalUp = rml::Vector4(_localUp, 1.0f) * rotateOZ;
	rml::Vector4 up = (rotateLocalUp * _worldMatrix).Normalize();
	_up = rml::Vector3(up.x, up.y, up.z);

	rml::Vector4 localTarget(0.0f, 0.0f, -(_target - _position).Length(), 1.0f);
	rml::Vector4 rotatedTarget = localTarget * rotateOZ;
	rml::Vector4 target = rotatedTarget * _worldMatrix;
	_target = rml::Vector3(target.x, target.y, target.z);

	UpdateWorldView();

	// go back to initial pos before starting rotation
	TranslateToPosition(goBackPos);
}

void Camera::DrawFrustrum() {

	_frustrum->GenerateCorners(this);
	_frustrum->Draw();
}

// PRIVATE

void Camera::UpdateAxis() {
	_yAxis = _up.Normalize();
	_zAxis = -(_target - _position).Normalize();
	_xAxis = (_zAxis.Cross(_yAxis)).Normalize();
}

void Camera::UpdateWorldView() {
	UpdateAxis();
	UpdateWorldMatrix();
	UpdateViewMatrix();

	_frustrum->GenerateCorners(this);
}

void Camera::UpdateWorldMatrix() {
	//rotation matrix
	_R.m[0][0] = _xAxis.x;
	_R.m[0][1] = _xAxis.y;
	_R.m[0][2] = _xAxis.z;
	_R.m[0][3] = 0;

	_R.m[1][0] = _yAxis.x;
	_R.m[1][1] = _yAxis.y;
	_R.m[1][2] = _yAxis.z;
	_R.m[1][3] = 0;

	_R.m[2][0] = _zAxis.x;
	_R.m[2][1] = _zAxis.y;
	_R.m[2][2] = _zAxis.z;
	_R.m[2][3] = 0;

	_R.m[3][0] = 0;
	_R.m[3][1] = 0;
	_R.m[3][2] = 0;
	_R.m[3][3] = 1;

	//transpose matrix
	_T.SetTranslation(_position.x, _position.y, _position.z);

	_worldMatrix = _R * _T;
}

void Camera::UpdateViewMatrix() {
	//rotation matrix
	_R.m[0][0] = _xAxis.x;
	_R.m[1][0] = _xAxis.y;
	_R.m[2][0] = _xAxis.z;
	_R.m[3][0] = 0;

	_R.m[0][1] = _yAxis.x;
	_R.m[1][1] = _yAxis.y;
	_R.m[2][1] = _yAxis.z;
	_R.m[3][1] = 0;

	_R.m[0][2] = _zAxis.x;
	_R.m[1][2] = _zAxis.y;
	_R.m[2][2] = _zAxis.z;
	_R.m[3][2] = 0;

	_R.m[0][3] = 0;
	_R.m[1][3] = 0;
	_R.m[2][3] = 0;
	_R.m[3][3] = 1;

	//transpose matrix
	_T.SetTranslation(-_position.x, -_position.y, -_position.z);

	_viewMatrix = _T * _R;
}

void Camera::TranslateToOrigin() {
	_target -= _position;
	_position -= _position;

	UpdateWorldView();
}

void Camera::TranslateToPosition(rml::Vector3 position) {
	TranslateToOrigin();

	_target += position;
	_position += position;

	UpdateWorldView();
}
