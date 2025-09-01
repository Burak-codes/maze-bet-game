# maze-bet-game
Labirent Oyunu (Binary Expression Tree Karar Mekanizması, C Dili)


# 🎮 Binary Expression Tree Labirent Oyunu (C)

Bu proje, **Binary Expression Tree (BET)** veri yapısını kullanarak bir karar mekanizması tasarlamayı öğretmek için hazırlanmış basit bir labirent oyunudur.

---

## 🧠 Amaç
Oyuncu **P** karakterini hareket ettirerek **E** çıkış karesine ulaşmaya çalışır.
Ama çıkışa varabilmek için şu kuralın sağlanması gerekir:




Bu sayede:
- **BET ağacını** kurmayı ve evaluate etmeyi
- Koşulları güncellemeyi
- Karar verme mekanizmasını kodla ifade etmeyi öğrenmiş olursun.

---

## 🎮 Oynanış
| Sembol | Anlamı |
|-------|--------|
| **P** | Oyuncu |
| **K** | Anahtar (üzerine gelince alınır → AnahtarVar=true) |
| **D** | Kapı (anahtar alınmadan geçilmez, anahtar alınca kalkar → KapiAcik=true) |
| **E** | Çıkış (kurallar sağlanıyorsa kazanırsın) |
| **#** | Duvar |

### Kontroller
- **W / A / S / D** → Hareket  
- **T** → `TuzakAtlat` değişkenini aç/kapat (kolay mod)  
- **R** → Reset (oyunu baştan başlatır)  
- **Q** → Çıkış  

---

## 🛠 Derleme
### Linux / macOS:
```bash
gcc -std=c11 -O2 -Wall -o maze_cli maze_cli.c
./maze_cli
Windows (MinGW):
gcc -std=c11 -O2 -Wall -o maze_cli.exe maze_cli.c
maze_cli.exe
📚 Bu Ödevin Bana Öğrettikleri

Binary Expression Tree (BET) kavramını gerçek bir senaryoda uygulamak

Karar mekanizması ve durum yönetimi pratiği

Basit bir oyun döngüsü tasarımı (input → update → draw)

Bu proje sayesinde veri yapıları dersi daha anlaşılır ve eğlenceli hale geldi.
