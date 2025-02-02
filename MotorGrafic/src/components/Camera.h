#pragma once

#include <utils/Math.h>

class Frustrum;

struct ProjectionInfo {
    float FoV;
    float Znear;
    float Zfar;
    float AspectRatio;
    float Width;
    float Height;
    bool IsPerspective;
};

class Camera{

public:
    /**
     * Default constructor. If you use this one you have to
     * call Init method before using camera!
     */
    Camera(unsigned int id, std::string name = "");
    /**
     * This cosntructor sets some default alues for the camera.
     * You can use the camera without calling init method. But you can
     * call init method to overrwrite default values.
     * 
     * \param id the ID of the camera
     * \param position initial position
     * \param target lookat target
     * \param up internal up vector
     * \param name
     */
    Camera(unsigned int id, rml::Vector3 position, rml::Vector3 target, rml::Vector3 up, std::string name="");
    ~Camera();

    void Init(rml::Vector3 position, rml::Vector3 target, rml::Vector3 up, float fov, float nearClip, float farClip);

    void SetPerspective(float fov, float widht, float height, float nearClip, float farClip);

    // Movement on local axis
    void MoveForward(float deltaTime);
    void MoveBackward(float deltaTime);
    void MoveRight(float deltaTime);
    void MoveLeft(float deltaTime);
    void MoveUp(float deltaTime);
    void MoveDown(float deltaTime);
    // Rotate (FPS style)
    void RotateOX(float deltaTime);
    void RotateOY(float deltaTime);
    void RotateOZ(float deltaTime);

    void DrawFrustrum();
    
    inline rml::Matrix GetProjectionMatrix() { return _projMatrix; }
    inline rml::Matrix GetViewmatrix() { return _viewMatrix; }
    inline rml::Matrix GetVP() { return _viewMatrix * _projMatrix; }

    inline rml::Vector3 GetPosition() { return _position; }

    inline rml::Vector3 GetForward() { return -_zAxis; }
    inline rml::Vector3 GetRight() { return _xAxis; }
    inline rml::Vector3 GetTarget() { return _target; }
    inline rml::Vector3 GetUp() { return _up; }

    inline ProjectionInfo GetProjectionInfo() { return _projectionInfo; }

    inline std::string GetName() { return _name; }

    inline Frustrum* GetFrustrum() { return _frustrum; }
private:
    void UpdateAxis();
    void UpdateWorldView();
    void UpdateWorldMatrix();
    void UpdateViewMatrix();


    void TranslateToOrigin();
    void TranslateToPosition(rml::Vector3 position);

private:
    rml::Vector3 _localUp = rml::Vector3(0.0f, 1.0f, 0.0f);

    ProjectionInfo _projectionInfo;

    rml::Vector3 _position;
    rml::Vector3 _target;
    rml::Vector3 _up;
    rml::Vector3 _rotation;

    rml::Vector3 _xAxis;
    rml::Vector3 _yAxis;
    rml::Vector3 _zAxis;

    rml::Matrix _worldMatrix; 
    rml::Matrix _viewMatrix;; 
    rml::Matrix _projMatrix;

    rml::Matrix _R;
    rml::Matrix _T;

    float _moveSpeed = 1.0;
    float _rotationSpeedHorizontal = 1.0f;
    float _rotationSpeedVertical = 1.0f;

    unsigned int _id;
    std::string _name;

    Frustrum* _frustrum;
};
