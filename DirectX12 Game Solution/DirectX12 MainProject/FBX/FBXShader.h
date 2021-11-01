#pragma once

class FBXShader {
public:
	void LoadPS(const char* filePath, const char* entryFuncName);

	void LoadVS(const char* filePath, const char* entryFuncName);

private:
	void Load(const char* filePath, const char* entryFuncName, const char* shaderModel);

private:
	ID3DBlob* m_blob = nullptr;
	bool m_isInited = false;
};