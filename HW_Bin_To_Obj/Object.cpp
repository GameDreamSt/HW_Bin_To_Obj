
#include <Object.h>

Quad::Quad()
{
	vertIds[0] = vertIds[1] = vertIds[2] = vertIds[3] = -1;
	texVertIds[0] = texVertIds[1] = texVertIds[2] = texVertIds[3] = -1;

	memset(&normals, 0, 12);

	textureName = "";
	glow = 0;

	opacityMode = "";
	opacityStrength = 0;

	enviroMapTex = "";
	enviroMapStrength = 0;
}

Tris::Tris()
{
	texVertIds[0] = -1;

	textureName = "";
	glow = 0;

	opacityMode = "";
	opacityStrength = 0;

	enviroMapTex = "";
	enviroMapStrength = 0;
}

ObjObject::ObjObject()
{
	name = "";
}

Vertex2D::Vertex2D()
{
	u = v = x = y = z = 0;
}

FaceRenderList::FaceRenderList()
{
	FirstQuad = 0;
	FirstTri = 0;
	NumTris = 0;
	OpType = TransType::TT_NONE;
	NumQuads = 0;
	SelfIllumination = false;
	ZWrite = true;
}

TextureBlockInfo::TextureBlockInfo()
{
	name = "";
	NumLists = 0;
}

TextureFaceList::TextureFaceList()
{
}

EnvironmentMap::EnvironmentMap()
{
	LateralShift = 0.f;
	name = "";
}

FaceList::FaceList()
{
	NumBlocks = CurrBlock = 0;
	CurrQuad = NumQuads = 0;
	CurrTri = NumTris = 0;
}

EnviroTri::EnviroTri()
{
	Shine = 0;
}

EnviroQuad::EnviroQuad()
{
	Shine = 0;
}

TextureTri::TextureTri()
{
	Glow = false;
	Opacity = 0.f;
	OpType = TransType::TT_NONE;
}

TextureQuad::TextureQuad()
{
	Glow = false;
	Opacity = 0.f;
	OpType = TransType::TT_NONE;
}

unsigned long RReadULong(vector<unsigned char>& allData, int index)
{
	return *(unsigned long*)&allData[index];
}

unsigned int RReadUInt(vector<unsigned char>& allData, int index)
{
	return *(unsigned int*)&allData[index];
}

float RReadFloat(vector<unsigned char>& allData, int index)
{
	return *(float*)&allData[index];
}

int RReadInt(vector<unsigned char>& allData, int index)
{
	return *(int*)&allData[index];
}

bool FaceList::Load(vector<unsigned char>& allData, int &index)
{
	NumBlocks = RReadUInt(allData, index); index += 4;
	CurrQuad = RReadUInt(allData, index); index += 4;
	NumQuads = RReadUInt(allData, index); index += 4;
	CurrTri = RReadUInt(allData, index); index += 4;
	NumTris = RReadUInt(allData, index); index += 4;

	Block.resize(NumBlocks);

	for (int i = 0; i < NumBlocks; i++)
		Block[i].Load(allData, index);

	return true;
}

bool BlockSection::Load(vector<unsigned char>& allData, int &index)
{
	FirstQuad = RReadULong(allData, index); index += 4;
	NumQuads = RReadULong(allData, index); index += 4;
	FirstTri = RReadULong(allData, index); index += 4;
	NumTris = RReadULong(allData, index); index += 4;

	return true;
}

bool TextureFaceList::Load(vector<unsigned char>& allData, int& index)
{
	unsigned int i;
	int size;

	FaceList::Load(allData, index);

	Tris.resize(NumTris);
	Quads.resize(NumQuads);

	if (NumTris)
		for (i = 0; i < NumTris; i++)
		{
			size = offsetof(TextureTri, OpType) + sizeof(Tris[i].OpType);
			memcpy(&Tris[i], &allData[index], size);
			index += size;
		}

	if (NumQuads)
		for (i = 0; i < NumQuads; i++)
		{
			size = offsetof(TextureQuad, OpType) + sizeof(Quads[i].OpType);
			memcpy(&Quads[i], &allData[index], size);
			index += size;
		}

	Info.resize(NumBlocks);

	for (i = 0; i < NumBlocks; i++)
		Info[i].Load(allData, index);

	return true;
}

bool EnvironmentMap::Load(vector<unsigned char>& allData, int& index)
{
	int Len, size;

	FaceList::Load(allData, index);

	Tris.resize(NumTris);
	Quads.resize(NumQuads);

	if (NumTris)
	{
		size = sizeof(EnviroTri) * NumTris;
		memcpy(&Tris[0], &allData[index], size);
		index += size;
	}

	if (NumQuads)
	{
		size = sizeof(EnviroQuad) * NumQuads;
		memcpy(&Quads[0], &allData[index], size);
		index += size;
	}

	Len = RReadInt(allData, index); index += 4;

	if (Len)
	{
		name.resize(Len);
		memcpy(&name[0], &allData[index], Len);

		index += Len + 1;
	}

	return true;
}

bool TextureBlockInfo::Load(vector<unsigned char>& allData, int& index)
{
	int Len = RReadInt(allData, index); index += 4;

	if (Len)
	{
		name.resize(Len);
		memcpy(&name[0], &allData[index], (size_t)Len + 1);

		index += Len + 1;
	}

	NumLists = RReadULong(allData, index); index += 4;

	List.resize(NumLists);

	int size = sizeof(FaceRenderList) * NumLists;
	memcpy(&List[0], &allData[index], size);
	index += size;

	return true;
}

Matrix::Matrix()
{
	Normal = false;
	m[0] = Vertex();
	m[1] = Vertex();
	m[2] = Vertex();
	t = Vertex();
}

ObjBlock::ObjBlock()
{
	Name = "";
	Num = 0;
	Size = 0;
	Visible = false;
	CoverCheckEnabled = false;
	FirstVert = 0;
	NumVerts = 0;
	FirstLVert = 0;
	NumLVerts = 0;
	StaticTM = Matrix();
}

Keyframe::Keyframe()
{
	M = Matrix();
	Frame = 0;
}

MatrixObjBlock::MatrixObjBlock()
{
	NumKeys = 0;
}

bool Matrix::Load(vector<unsigned char>& allData, int& index)
{
	*this = *(Matrix*)(&allData[index]);
	index += sizeof(Matrix);
	return true;
}

bool ObjBlock::Load(vector<unsigned char>& allData, int& index)
{
	int Len = RReadInt(allData, index); index += 4;

	if (Len)
	{
		int size = Len + 1;
		Name.resize(Len);
		memcpy(&Name[0], &allData[index], size);
		index += size;
	}

	Num = RReadUInt(allData, index); index += 4;

	StaticTM.Load(allData, index);

	Size = RReadUInt(allData, index); index += 4;

	FirstVert = RReadULong(allData, index); index += 4;
	NumVerts = RReadULong(allData, index); index += 4;
	FirstLVert = RReadULong(allData, index); index += 4;
	NumLVerts = RReadULong(allData, index); index += 4;

	Len = RReadInt(allData, index); index += 4;

	if(Len)
		index += Len + 1; // Skip material

	return true;
}

bool MatrixObjBlock::Load(vector<unsigned char>& allData, int& index, unsigned long AnimFrames)
{
	ObjBlock::Load(allData, index);

	int Bool = RReadInt(allData, index); index += 4;
	int size;

	if (Bool)
	{
		Anims.resize(AnimFrames);
		size = AnimFrames * sizeof(Matrix);
		memcpy(&Anims[0], &allData[index], size);
		index += size;
	}

	NumKeys = RReadULong(allData, index); index += 4;

	if (NumKeys)
	{
		Keys.resize(NumKeys);
		size = NumKeys * sizeof(Keyframe);
		memcpy(&Keys[0], &allData[index], size);
		index += size;
	}

	return true;
}

bool MorphObjBlock::Load(vector<unsigned char>& allData, int& index)
{
	ObjBlock::Load(allData, index);

	NumKeys = RReadInt(allData, index); index += 4;
	if (NumKeys)
	{
		Keys.resize(NumKeys);
		int size = NumKeys * sizeof(KeyVerts);
		memcpy(&Keys[0], &allData[index], size);
		index += size;
	}

	return true;
}

bool SuperObjBlock::Load(vector<unsigned char>& allData, int& index, unsigned long AnimFrames)
{
	ObjBlock::Load(allData, index);

	numKeyVerts = RReadInt(allData, index); index += 4;

	if (numKeyVerts > 1) // Morph mode
	{
		keyVerts.resize(numKeyVerts);
		int size = numKeyVerts * sizeof(KeyVerts);
		memcpy(&keyVerts[0], &allData[index], size);
		index += size;
		return true;
	}

	int size;

	if (numKeyVerts)
	{
		numKeyVerts = 0;

		Anims.resize(AnimFrames);
		size = AnimFrames * sizeof(Matrix);
		memcpy(&Anims[0], &allData[index], size);
		index += size;
	}

	numKeyFrames = RReadULong(allData, index); index += 4;

	if (numKeyFrames)
	{
		keyFrames.resize(numKeyFrames);
		size = numKeyFrames * sizeof(Keyframe);
		memcpy(&keyFrames[0], &allData[index], size);
		index += size;
	}

	return true;
}
