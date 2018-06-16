//���� ��ü�� ������ ���� ��� ���۸� �����Ѵ�. 
cbuffer cbGameObjectInfo : register(b0) 
{
	matrix gmtxWorld : packoffset(c0); 
};
//ī�޶��� ������ ���� ��� ���۸� �����Ѵ�. 
cbuffer cbCameraInfo : register(b1) {
	matrix gmtxView : packoffset(c0); 
	matrix gmtxProjection : packoffset(c4); 
};

//���� ���̴��� �Է��� ���� ����ü�� �����Ѵ�. 
struct VS_INPUT { 
	float3 position : POSITION; 
	float4 color : COLOR; 
};
//���� ���̴��� ���(�ȼ� ���̴��� �Է�)�� ���� ����ü�� �����Ѵ�. 
struct VS_OUTPUT 
{
	float4 position : SV_POSITION;
	float4 color : COLOR; 
};
//���� ���̴��� �����Ѵ�.
VS_OUTPUT VSMain(VS_INPUT input) 
{ 
	VS_OUTPUT output;
//������ ��ġ ���ʹ� ������ǥ��� ǥ���Ǿ� �����Ƿ� ��ȯ���� �ʰ� �״�� ����Ѵ�.
	output.position = float4(input.position, 1.0f);
	//�ԷµǴ� �ȼ��� ����(�����Ͷ����� �ܰ迡�� �����Ͽ� ���� ����)�� �״�� ����Ѵ�. 
	output.color = input.color;

	return(output);
}

//���� ���̴��� �����Ѵ�.
VS_OUTPUT VSDiffused(VS_INPUT input) 
{
	VS_OUTPUT output; //������ ��ȯ(���� ��ȯ, ī�޶� ��ȯ, ���� ��ȯ)�Ѵ�. 
	output.position = mul(mul(mul(float4(input.position, 1.0f), gmtxWorld), gmtxView), gmtxProjection);	//ī�޶� ��ǥ���� �ǹ̸� ���� 4�������ͷ� �ٲ��.
	output.color = input.color;	//������ �ٲ�� ����. ������ �� ����.
	return(output);
}
//�ȼ� ���̴��� �����Ѵ�. 
float4 PSDiffused(VS_OUTPUT input) : SV_TARGET { return(input.color); }
//�ȼ� ���̴��� �����Ѵ�. 
float4 PSMain(VS_OUTPUT input) : SV_TARGET
{ 
	// ������ �� ������ �����Ͷ������κ��� �Ѿ�� ����.
	//�ԷµǴ� �ȼ��� ������ �״�� ���-���� �ܰ�(���� Ÿ��)�� ����Ѵ�. 
	return(input.color); 
}