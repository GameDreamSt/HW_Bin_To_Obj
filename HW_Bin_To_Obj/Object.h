#pragma once

#include <string>
#include <vector>
#include <Vectors.h>

using namespace std;

class Quad
{
public:
    Quad();

    int vertIds[4];
    int texVertIds[4];
    float normals[12];

    string textureName;
    float glow;
    Vector3 flat;

    string opacityMode;
    float opacityStrength;

    string enviroMapTex;
    float enviroMapStrength;
};

class Tris
{
public:
    Tris();

    int vertIds[3];
    int texVertIds[3];
    float normals[9];

    string textureName;
    float glow;
    Vector3 flat;

    string opacityMode;
    float opacityStrength;

    string enviroMapTex;
    float enviroMapStrength;
};

class ObjObject
{
public:
    ObjObject();

    string name;

    int frameAnimStart;

    Vector3 position;

    vector<Vector3> verts; // x y z
    vector<pair<float, float>> texVerts; // uvs

    vector<Quad> quads;
    vector<Tris> tris;

    vector<string> textureFilenames;
};

#define	BASE_OBJECT_SPECIFIC_DATA_COUNT	64
#define MAX_FACELISTS 3

#define FACES_BUMP 0
#define FACES_TEXTURE 1
#define FACES_REFLECT 2

typedef struct
{
    unsigned short ObjectNo;
    unsigned short BlockNo;
} ExtraVertInf;

typedef struct
{
    unsigned short Flags;
    unsigned short Count;
} ContactInf;

class Vertex
{
public:
    union
    {
        float x;
        unsigned long lx;
    };
    union
    {
        float y;
        unsigned long ly;
    };
    union
    {
        float z;
        unsigned long lz;
    };
    union
    {
        float w;
        float rhw;
        unsigned long lw;
        unsigned long Flags;
        ContactInf ContactInfo;
        ExtraVertInf inf;
    };
};

class ScreenVertex
{
public:
    Vertex v;
    unsigned long ClipFlags;
};

class TransVert
{
public:
    Vertex V;
    ScreenVertex SV;
    float u, v;
};

typedef Vertex Colour;
typedef Vertex Vector;

class LightVert
{
public:
    TransVert* TV;
    Vector N;
    Colour C;
    unsigned long VIndex;
};

class BlockSection
{
public:
    unsigned long FirstQuad, NumQuads;
    unsigned long FirstTri, NumTris;

    bool Load(vector<unsigned char>& allData, int& index);

    BlockSection() : FirstQuad(0), NumQuads(0), FirstTri(0), NumTris(0) {};
};

class FaceList
{
protected:
    int NumBlocks, CurrBlock;

public:
    vector<BlockSection> Block;
    unsigned int CurrQuad, NumQuads;
    unsigned int CurrTri, NumTris;

    virtual bool Load(vector<unsigned char>& allData, int &index);

    FaceList();
};

class Triangle
{
public:
    unsigned short a, b, c, pad;
};

class Quadrilateral
{
public:
    unsigned short a, b, c, d; // Keep these in order
};

class Vertex2D
{
public:
    union
    {
        float x;
        float u;
    };
    union
    {
        float y;
        float z;
        float v;
    };

    Vertex2D();
};

typedef Vertex2D TextureVertex;

typedef enum { TT_INVALID, TT_NONE, TT_ALPHA, TT_ADD, TT_SUB, TT_SHADOW, TT_MULTIPLY, TT_MULTIPLY2X, TT_DEST_COLOUR, TT_SOURCE_COLOUR } TransType;

class TextureQuad : public Quadrilateral
{
public:
    TextureVertex TVert[4];
    unsigned long Glow;
    float Opacity;
    TransType OpType;

    TextureQuad();
};

class TextureTri : public Triangle
{
public:
    TextureVertex TVert[3];
    unsigned long Glow;
    float Opacity;
    TransType OpType;

    TextureTri();
};

class FaceRenderList
{
public:
    bool SelfIllumination;
    TransType OpType;
    bool ZWrite;
    unsigned long FirstQuad, NumQuads;
    unsigned long FirstTri, NumTris;

    FaceRenderList();
};

class TextureBlockInfo
{
public:
    string name;
    unsigned long NumLists;
    vector<FaceRenderList> List;

    TextureBlockInfo();

    bool Load(vector<unsigned char>& allData, int& index);
};

class TextureFaceList : public FaceList
{
public:
    vector<TextureBlockInfo> Info;
    vector<TextureTri> Tris;
    vector<TextureQuad> Quads;

    TextureFaceList();

    bool Load(vector<unsigned char>& allData, int& index) override;
};

class EnviroTri : public Triangle
{
public:
    unsigned long Shine;
    TextureVertex TV[3];

    EnviroTri();
};

class EnviroQuad : public Quadrilateral
{
public:
    unsigned long Shine;
    TextureVertex TV[4];

    EnviroQuad();
};

class EnvironmentMap : public FaceList
{
private:
    float LateralShift;

public:
    string name;
    static bool LoadLocalMaps;

    vector<EnviroTri> Tris;
    vector<EnviroQuad> Quads;

    EnvironmentMap();

    bool Load(vector<unsigned char>& allData, int& index) override;
};

// ******
// Matrix

class Matrix
{
public:
    Vertex m[3];
    Vertex t;
    bool Normal;

    bool Load(vector<unsigned char>& allData, int& index);

    Matrix();
};

class ObjBlock
{
public:
    string Name;
    unsigned int Num;

    Matrix StaticTM;
    float Size;
    bool Visible, CoverCheckEnabled;

    unsigned long FirstVert, NumVerts;		// These 4 must remain
    unsigned long FirstLVert, NumLVerts;	// next to each other

    virtual bool Load(vector<unsigned char>& allData, int& index);

    ObjBlock();
};

class Keyframe
{
public:
    Matrix M;
    unsigned long Frame;

    Keyframe();
};

class MatrixObjBlock : public ObjBlock
{
public:
    vector<Matrix> Anims;
    vector<Keyframe> Keys;
    unsigned long NumKeys;

    bool Load(vector<unsigned char>& allData, int& index, unsigned long AnimFrames);

    MatrixObjBlock();
};

class KeyVerts
{
public:
    unsigned long Keyframe;

    unsigned long FirstVert;
    unsigned long FirstLVert;
};

class MorphObjBlock : public ObjBlock
{
public:
    vector<KeyVerts> Keys;
    int NumKeys;

    bool Load(vector<unsigned char>& allData, int& index);

    MorphObjBlock() : ObjBlock(), Keys(), NumKeys(0) {};
};

class SuperObjBlock : public ObjBlock
{
public:
    vector<Matrix> Anims;
    vector<Keyframe> keyFrames;
    unsigned long numKeyFrames;

    // Morph info:
    int numKeyVerts;
    vector<KeyVerts> keyVerts;

    bool Load(vector<unsigned char>& allData, int& index, unsigned long AnimFrames);

    SuperObjBlock() : ObjBlock(), numKeyVerts(0), numKeyFrames(0){};
};