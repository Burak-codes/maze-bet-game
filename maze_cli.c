// maze_cli.c — BET Labirent (C, ASCII, harici kütüphane yok)
// Kontroller: W/A/S/D = hareket | T: TuzakAtlat toggle | R: reset | Q: çıkış

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <string.h>

// ---------------- Ayarlar ----------------
#define W  40     // harita genişlik (kolay görünürlük için)
#define H  20     // harita yükseklik
#define WALLS 120 // rastgele duvar sayısı
#define SEED  42  // sabit seed (tekrar üretilebilir)

/* Harita sembolleri
   '#': duvar
   ' ': boş
   'P': oyuncu
   'K': anahtar
   'D': kapı
   'E': çıkış
*/
enum { EMPTY=' ', WALL='#', PLAYER='P', KEY='K', DOOR='D', EXIT='E' };

// ---------------- BET (Binary Expression Tree) ----------------
typedef enum { NODE_VAR=0, NODE_AND, NODE_OR, NODE_NOT } NodeType;

typedef struct BETNode {
    NodeType type;
    const char* varName;     // sadece NODE_VAR için
    struct BETNode* left;
    struct BETNode* right;
} BETNode;

typedef struct {
    bool AnahtarVar;
    bool KapiAcik;    // 'KapıAçık' sadeleştirilmiş
    bool TuzakAtlat;
} Conditions;

static bool evalVar(const Conditions* c, const char* name) {
    if (strcmp(name, "AnahtarVar")==0) return c->AnahtarVar;
    if (strcmp(name, "KapıAçık")==0 || strcmp(name, "KapiAcik")==0) return c->KapiAcik;
    if (strcmp(name, "TuzakAtlat")==0) return c->TuzakAtlat;
    return false;
}

static bool betEvaluate(const BETNode* n, const Conditions* c) {
    if (!n) return false;
    if (n->type == NODE_VAR) return evalVar(c, n->varName);
    if (n->type == NODE_NOT) {
        const BETNode* child = n->right ? n->right : n->left;
        return !(child ? betEvaluate(child, c) : false);
    }
    bool lv = n->left  ? betEvaluate(n->left,  c) : false;
    bool rv = n->right ? betEvaluate(n->right, c) : false;
    if (n->type == NODE_AND) return lv && rv;
    if (n->type == NODE_OR)  return lv || rv;
    return false;
}

static BETNode* makeVar(const char* name) {
    BETNode* n = (BETNode*)calloc(1, sizeof(BETNode));
    n->type = NODE_VAR; n->varName = name; return n;
}
static BETNode* makeNode(NodeType t, BETNode* L, BETNode* R) {
    BETNode* n = (BETNode*)calloc(1, sizeof(BETNode));
    n->type = t; n->left = L; n->right = R; return n;
}
static void freeBET(BETNode* n){ if(!n) return; freeBET(n->left); freeBET(n->right); free(n); }

// ---------------- Oyun durumu ----------------
typedef struct {
    char grid[H][W];
    int px, py;          // oyuncu konumu
    int kx, ky;          // anahtar konumu
    int dx, dy;          // kapı konumu
    int ex, ey;          // çıkış konumu
    Conditions cond;
    BETNode* exitRule;
} Game;

static void clearScreen(void) {
    // taşınabilir basit temizleme (çok satır bas)
    for (int i=0;i<30;i++) puts("");
}

static void draw(const Game* g) {
    clearScreen();
    // üst kenar
    putchar('+'); for(int x=0;x<W;x++) putchar('-'); puts("+");
    for (int y=0; y<H; y++) {
        putchar('|');
        for (int x=0; x<W; x++) {
            putchar(g->grid[y][x]);
        }
        puts("|");
    }
    putchar('+'); for(int x=0;x<W;x++) putchar('-'); puts("+");

    printf("Anahtar: %s | Kapi: %s | TuzakAtlat: %s\n",
           g->cond.AnahtarVar ? "Var":"Yok",
           g->cond.KapiAcik   ? "Acik":"Kapali",
           g->cond.TuzakAtlat ? "ON":"OFF");

    bool ok = betEvaluate(g->exitRule, &g->cond);
    printf("Kural: (AnahtarVar AND KapiAcik) OR TuzakAtlat  ->  %s\n", ok ? "OK" : "X");
    puts("W/A/S/D: hareket | T: TuzakAtlat toggle | R: reset | Q: cikis");
}

static void fillWithSpaces(Game* g){
    for(int y=0;y<H;y++) for(int x=0;x<W;x++) g->grid[y][x]=EMPTY;
}

static bool inBounds(int x,int y){ return x>=0 && y>=0 && x<W && y<H; }

static void placeWalls(Game* g, int count) {
    // rastgele duvarlar; spawn/exit/kapı/anahtar üstüne konmaz
    for (int i=0;i<count;i++) {
        int x = rand() % W;
        int y = rand() % H;
        if ((x==g->px && y==g->py) || (x==g->ex && y==g->ey) || (x==g->dx && y==g->dy) || (x==g->kx && y==g->ky)) {
            i--; continue;
        }
        g->grid[y][x] = WALL;
    }
}

static BETNode* buildExitRule(void) {
    // (AnahtarVar AND KapıAçık) OR TuzakAtlat
    BETNode* leftAND = makeNode(NODE_AND, makeVar("AnahtarVar"), makeVar("KapıAçık"));
    BETNode* root    = makeNode(NODE_OR, leftAND, makeVar("TuzakAtlat"));
    return root;
}

static void resetGame(Game* g) {
    fillWithSpaces(g);
    g->px = 1;  g->py = 1;
    g->ex = W-2; g->ey = H-2;
    g->dx = W/2; g->dy = H/2;
    g->kx = W-8; g->ky = 2;

    g->cond.AnahtarVar = false;
    g->cond.KapiAcik   = false;
    g->cond.TuzakAtlat = false;

    // kenarları duvar yap
    for(int x=0;x<W;x++){ g->grid[0][x]=WALL; g->grid[H-1][x]=WALL; }
    for(int y=0;y<H;y++){ g->grid[y][0]=WALL; g->grid[y][W-1]=WALL; }

    // sabit yerleştirmeler
    g->grid[g->py][g->px] = PLAYER;
    g->grid[g->ey][g->ex] = EXIT;
    g->grid[g->dy][g->dx] = DOOR;
    g->grid[g->ky][g->kx] = KEY;

    // rastgele duvarlar (spawn/exit/door/key üzerine koymuyoruz)
    placeWalls(g, WALLS);
}

static void initGame(Game* g) {
    memset(g, 0, sizeof(*g));
    g->exitRule = buildExitRule();
    resetGame(g);
}

static void destroyGame(Game* g) {
    freeBET(g->exitRule);
}

static bool isBlocked(const Game* g, int nx, int ny) {
    if (!inBounds(nx,ny)) return true;
    char c = g->grid[ny][nx];
    if (c == WALL) return true;
    if (c == DOOR && !g->cond.AnahtarVar) return true; // anahtar yoksa kapı engel
    return false;
}

static void applyCell(Game* g, int x, int y) {
    char c = g->grid[y][x];
    if (c == KEY) {
        g->cond.AnahtarVar = true;
        g->grid[y][x] = EMPTY;
        // kapıyı aç: kapıyı boşluğa çevir + KapiAcik=true
        g->grid[g->dy][g->dx] = EMPTY;
        g->cond.KapiAcik = true;
    }
}

static bool atExitAndOk(const Game* g){
    return (g->px == g->ex && g->py == g->ey) && betEvaluate(g->exitRule, &g->cond);
}
static bool atExitButNotOk(const Game* g){
    return (g->px == g->ex && g->py == g->ey) && !betEvaluate(g->exitRule, &g->cond);
}

static void movePlayer(Game* g, int dx, int dy) {
    int nx = g->px + dx, ny = g->py + dy;
    if (isBlocked(g, nx, ny)) return;

    // mevcut hücreyi boşalt
    g->grid[g->py][g->px] = EMPTY;

    // yeni hücrenin etkisini uygula (anahtar vb.)
    applyCell(g, nx, ny);

    // yeni konum
    g->px = nx; g->py = ny;
    g->grid[g->py][g->px] = PLAYER;
}

int main(void) {
    srand(SEED); // istersen time(NULL) yapıp rastgeleleştir
    Game g;
    initGame(&g);

    while (1) {
        draw(&g);

        if (atExitAndOk(&g)) {
            puts("\nTebrikler! Cikis kurali saglandi ve labirent cozuldu!");
            puts("Devam icin R (reset) ya da Q (cikis) girin.");
        } else if (atExitButNotOk(&g)) {
            puts("\nCikis karesine ulastin ama kural saglanmiyor! (Game Over)");
            puts("Devam icin R (reset) ya da Q (cikis) girin.");
        }

        printf("\nKomut: ");
        int ch = getchar();
        // input'tan sonra satır sonunu temizle
        if (ch!='\n') { int c; while((c=getchar())!='\n' && c!=EOF){} }

        if (ch=='q' || ch=='Q') break;
        if (ch=='r' || ch=='R') { resetGame(&g); continue; }
        if (ch=='t' || ch=='T') { g.cond.TuzakAtlat = !g.cond.TuzakAtlat; continue; }

        if (atExitAndOk(&g) || atExitButNotOk(&g)) {
            // oyun bitmişken yön girişlerini yoksay
            continue;
        }

        if (ch=='w' || ch=='W') movePlayer(&g, 0, -1);
        else if (ch=='s' || ch=='S') movePlayer(&g, 0, 1);
        else if (ch=='a' || ch=='A') movePlayer(&g, -1, 0);
        else if (ch=='d' || ch=='D') movePlayer(&g, 1, 0);
        // aksi halde diğer tuşları yok say
    }

    destroyGame(&g);
    return 0;
}
