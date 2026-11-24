#include "../Tool.h"

#define ILLUSION_TAU 6.28318530717958647692
#define ILLUSION_SMOOTH_MAX 4 // most samples per pixel along each axis

// --------------------------------   Illusion State    -------------------------------- //

enum ILLUSION_Type {
    ILLUSION_CafeWall = 0, ILLUSION_Hermann = 1, ILLUSION_Scintillating = 2, // grids
    ILLUSION_Zollner = 3, ILLUSION_Hering = 4, ILLUSION_Poggendorff = 5, // tilt and alignment
    ILLUSION_MullerLyer = 6, ILLUSION_Ponzo = 7, ILLUSION_Ebbinghaus = 8, ILLUSION_Jastrow = 9, // size
    ILLUSION_Kanizsa = 10, // illusory contours
    ILLUSION_White = 11, ILLUSION_Munker = 12, ILLUSION_Contrast = 13, ILLUSION_Chevreul = 14, ILLUSION_Cornsweet = 15, // brightness and color
    ILLUSION_Fraser = 16, ILLUSION_Pinna = 17, ILLUSION_Drift = 18, ILLUSION_Ouchi = 19, ILLUSION_Bulge = 20, // apparent motion and shape
    ILLUSION_Moire = 21, ILLUSION_Lilac = 22, // interference and afterimages
    ILLUSION_Count = 23
};

enum ILLUSION_Color { ICOLOR_Dark = 0, ICOLOR_Light = 1, ICOLOR_Accent = 2, ICOLOR_Mid = 3 };

enum ILLUSION_Mix { IMIX_None = 0, IMIX_Blend = 1, IMIX_Add = 2, IMIX_Diff = 3, IMIX_Mult = 4, IMIX_Xor = 5 };

typedef struct {
    int type;
    unsigned colors[4]; // dark, light, accent, and mid colors, see ILLUSION_Color
    int count[2]; // repetitions across x y, or elements and rings
    double size; // relative size of the main elements
    double strength; // how strong the inducing pattern is, 0 removes the effect
    double thickness; // line, mortar, and street widths
    double angle; // rotation of the whole figure in degrees
    double zoom;
    double phase; // radians, animated by frames/encode
    int reveal; // removes or marks the inducing pattern to prove what is really there
    int smooth; // samples per pixel along each axis
    int mix; // combination with applied image
} Illusion_State;

extern Illusion_State _illusion;

// --------------------------------   Illusion Catalogue (see Illusion.c)    -------------------------------- //

typedef struct {
    const char* name;
    const char* about; // what to look for
    const char* revealed; // what reveal changes, NULL when the figure has nothing to reveal
    int count[2];
    double size, strength, thickness, angle;
    unsigned colors[4];
    int usesPhase; // TRUE when phase moves the figure, otherwise animation sweeps strength
} Illusion_Info;

extern const Illusion_Info _illusionInfo[ILLUSION_Count]; // indexed by ILLUSION_Type

void illusionPreset(Illusion_State* state, int type); // loads the recommended settings of an illusion

// --------------------------------   Image Operations (see Illusion.c)    -------------------------------- //

unsigned illusionColor(double u, double v); // color at a point, u spans the aspect ratio and v spans -0.5 to 0.5
Rasteron_Image* illusionImgOp(ImageSize size); // renders the current state with smoothing
