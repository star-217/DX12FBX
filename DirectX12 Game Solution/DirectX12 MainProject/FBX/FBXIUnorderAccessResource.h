
class FBXIUnorderAccessResrouce : public FBXIShaderResource {
public:
	virtual ~FBXIUnorderAccessResrouce() {};
	/// <summary>
	/// UAVÇ…ìoò^ÅB
	/// </summary>
	/// <param name=""></param>
	/// <param name="bufferNo"></param>
	virtual void RegistUnorderAccessView(D3D12_CPU_DESCRIPTOR_HANDLE, int bufferNo) = 0;
};