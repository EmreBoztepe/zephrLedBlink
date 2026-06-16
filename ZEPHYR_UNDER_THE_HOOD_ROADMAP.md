# Zephyr Under the Hood Roadmap

Bu dosya, bu projede Zephyr'i sadece kullanmak yerine "alt tarafta ne oluyor?"
sorusuyla öğrenmek için hazırlanmış çalışma planıdır.

Ana hedef:

```text
Uygulama kodu -> Devicetree -> binding YAML -> Kconfig -> generated headers
-> driver instance -> linker/flash image
```

zincirini adım adım görmek.

## Çalışma Kuralı

- Her aşamada önce kavramı anlayacağız, sonra küçük bir deney yapacağız.
- Büyük refactor yok. Mevcut `led_blink` projesi bozulmadan ilerleyeceğiz.
- Her DTS değişikliğinden sonra build çıktısında doğrulama yapacağız.
- Hata çıkarsa önce hatanın katmanını ayıracağız: DTS mi, binding mi, Kconfig mi,
  linker mı, yoksa uygulama kodu mu?

Kalıcı kaynak dosyalar:

```text
CMakeLists.txt
prj.conf
app.overlay
src/main.c
src/threadA.c
src/threadB.c
```

Build sonrası kontrol edilecek dosyalar:

```text
build/zephyr/zephyr.dts
build/zephyr/include/generated/zephyr/devicetree_generated.h
build/zephyr/.config
```

Not: `.config` kalıcı kaynak dosya değildir. İncelemek için kullanılır. Kalıcı
ayar gerekiyorsa genelde `prj.conf`, board defconfig veya Devicetree tarafına
yazılır.

## 0. Büyük Resim: Zephyr Build Akışı

Öğrenilecek kavramlar:

- `west build`
- CMake ve Zephyr package
- board seçimi
- Devicetree preprocessing
- Kconfig merge
- generated headers
- linker ve final image

Akış:

```text
west build -b <board>
-> board bulunur
-> SoC .dtsi + board .dts + app.overlay merge edilir
-> binding YAML dosyaları ile DTS doğrulanır
-> zephyr.dts ve devicetree_generated.h üretilir
-> board defconfig + prj.conf merge edilip .config oluşur
-> C kaynakları derlenir
-> linker flash/RAM yerleşimini yapar
-> zephyr.elf / zephyr.hex / zephyr.bin oluşur
```

Mini görev:

- Mevcut projeyi build ettikten sonra `build/zephyr/zephyr.dts` dosyasını aç.
- `app.overlay` içindeki `zephyr,user` ve `adc1` değişikliklerinin merged DTS
  içine girdiğini doğrula.

Başarı kriteri:

- `west` sadece build'i başlatıyor; DTS header üretimini Zephyr/CMake/DTC
  pipeline'ı yapıyor ayrımını açıklayabiliyoruz.

## 1. Devicetree Dilini Okuma

Öğrenilecek kavramlar:

- root node: `/ { ... };`
- node name: `led_0`
- node label: `green_led: led_0`
- property: `gpios = <...>;`
- node reference / phandle: `&gpioa`
- alias: `led0 = &green_led;`
- path: `/soc/gpio@...`

Mini görev:

- Board DTS veya merged DTS içinde bir LED node'u bul.
- Şu zinciri elle takip et:

```text
DT_ALIAS(led0)
-> /aliases içindeki led0
-> &green_led gibi node reference
-> gerçek LED node'u
-> gpios property
-> GPIO controller node
```

Production notu:

- Board üzerinde "default LED" gibi taşınabilir kavramlar için `aliases`
  kullanmak yaygın ve temizdir.
- Tek bir board'a özel hızlı denemede `DT_NODELABEL(...)` kullanılabilir, ama
  uygulamayı board bağımsız tutmak istiyorsak alias daha uygundur.

## 2. Phandle ve Phandle-Array

Öğrenilecek kavramlar:

- `phandle`: başka node'a saf referans.
- `phandle-array`: node referansı + o sağlayıcıya ait argüman hücreleri.
- specifier space
- `#gpio-cells`, `#pwm-cells`, `#clock-cells`

Örnek:

```dts
gpios = <&gpioa 5 GPIO_ACTIVE_HIGH>;
```

Burada:

```text
&gpioa              -> provider node
5                   -> pin number
GPIO_ACTIVE_HIGH    -> flags
```

Bu formatı tüketen taraf LED node'u değildir; formatı belirleyen provider
node'daki `#gpio-cells` bilgisidir.

Mini görev:

- `gpios = <...>` satırını parçala.
- Hangi kısmın phandle, hangi kısmın specifier cell olduğunu not et.

Başarı kriteri:

- "Bu property neden 3 parçadan oluşuyor?" sorusuna binding ve provider
  cells üzerinden cevap verebiliyoruz.

## 3. Binding YAML Dosyaları

Öğrenilecek kavramlar:

- `compatible`
- binding lookup
- `required: true`
- `type`
- `enum`
- `child-binding`

Ana fikir:

```dts
compatible = "gpio-leds";
```

satırı Zephyr'e "bu node şu binding kurallarına göre yorumlanacak" der.
Binding YAML dosyası ise hangi property'lerin zorunlu olduğunu ve tiplerini
tanımlar.

Mini görev:

- `gpio-leds` binding dosyasını bul ve oku.
- `gpios` property neden zorunlu, `label` neden opsiyonel ayır.
- Sonra ADC veya I2C bindinglerinden birini açıp `required: true` property'leri
  listele.

Production notu:

- DTS yazarken "kurs böyle yazdı" diye değil, binding dosyasındaki zorunlu
  property'lere bakarak ilerlemek doğru yöntemdir.

## 4. LED Blink Katmanlarını Takip Etme

Öğrenilecek kavramlar:

- GPIO API
- LED API
- `gpio_dt_spec`
- `GPIO_DT_SPEC_GET`
- `device_is_ready`
- driver readiness

Katmanlar:

```text
Application code
-> GPIO/LED API
-> Devicetree macro
-> gpios property
-> GPIO controller device
-> STM32 GPIO driver
-> gerçek MCU pini
```

Mini görev:

- Mevcut LED blink kodunda kullanılan macro'ları tek tek aç.
- `GPIO_DT_SPEC_GET` sonucunda oluşan yapının içinde ne var öğren:

```c
struct gpio_dt_spec {
    const struct device *port;
    gpio_pin_t pin;
    gpio_dt_flags_t dt_flags;
};
```

Başarı kriteri:

- C kodundaki `led.port`, `led.pin`, `led.dt_flags` değerlerinin DTS'ten nasıl
  geldiğini açıklayabiliyoruz.

## 5. Kconfig ve Devicetree Ayrımı

Öğrenilecek kavramlar:

- `CONFIG_GPIO=y`
- SoC driver seçimi
- `status = "okay"`
- `compatible` ile driver match
- `.config`

Ana ayrım:

```text
Kconfig     -> hangi yazılım parçaları derlensin?
Devicetree  -> hangi donanım var ve nasıl bağlı?
```

Önemli örnek:

```text
CONFIG_GPIO=y
```

tek başına MCU pinlerini kontrol etmeye yetmez. Ayrıca:

- SoC'nin GPIO driver'ı seçilmiş olmalı.
- DTS içinde GPIO controller node'u olmalı.
- Node `status = "okay"` olmalı.
- Node'un `compatible` string'i driver ile eşleşmeli.

Mini görev:

- `prj.conf` içindeki `CONFIG_GPIO=y` satırını `.config` içinde doğrula.
- Merged DTS içinde GPIO controller node'larını bul.

Production notu:

- Uygulama seviyesinde gereken subsystem ayarları `prj.conf` içinde tutulur.
- Board'un güvenli varsayılanları board defconfig içinde tutulur.

## 6. Custom Board Skeleton

Öğrenilecek kavramlar:

- out-of-tree board
- `board.yml`
- `Kconfig.<board>`
- `<board>_defconfig`
- `<board>.dts`
- `board.cmake`

Başlangıç hedefi:

```text
Kendi board tanımı ile sadece blinky derlensin.
```

Önerilen dosya yapısı:

```text
boards/<vendor>/<board_name>/
  board.yml
  Kconfig.<board_name>
  <board_name>_defconfig
  <board_name>.dts
  board.cmake
```

Mini görev:

- Önce mevcut `stm32f4_disco` üzerinden kopya çekmeden, dosyaların görevini
  anlayarak minimal skeleton çıkar.
- İlk denemede sadece board'un bulunması ve build'in başlaması hedeflensin.

Production notu:

- Hazır desteklenen board için production projede custom board oluşturmak şart
  değildir; overlay yeterlidir.
- Kendi PCB'n veya Zephyr'de olmayan kartın varsa production çözüm custom board
  tanımıdır.

## 7. /chosen Node ve Memory Tanımı

Öğrenilecek kavramlar:

- `/chosen`
- `zephyr,sram`
- `zephyr,flash`
- `zephyr,console`
- linker memory regions
- flash/RAM base ve size

Örnek:

```dts
chosen {
    zephyr,sram = &sram0;
    zephyr,flash = &flash0;
};
```

Memory node örneği:

```dts
&sram0 {
    reg = <0x20000000 DT_SIZE_K(40)>;
};

&flash0 {
    reg = <0x08000000 DT_SIZE_K(256)>;
};
```

Mini görev:

- Linker flash/RAM hatası gördüğünde önce `.config` içindeki size değerlerine
  bak.
- Sonra DTS içinde `/chosen` ve memory node'ları doğru mu kontrol et.

Production notu:

- Flash/RAM adreslerini uzun vadede `menuconfig` ile ayarlamak doğru yöntem
  değildir.
- Donanım gerçeği DTS içinde, board varsayılanları defconfig içinde olmalıdır.

## 8. Build Hatalarını Katmana Ayırma

Tipik hata türleri:

```text
DTS parse error              -> syntax veya include problemi
Binding validation error     -> required property eksik veya yanlış type
Kconfig warning/error        -> bağımlılık eksik veya yanlış config
Undefined reference          -> driver/subsystem derlenmemiş olabilir
Linker region overflow       -> flash/RAM size veya code size problemi
Device not ready             -> DTS node disabled veya driver init başarısız
```

Debug sırası:

1. Hata metninde dosya ve satır var mı?
2. Hata DTS mi, Kconfig mi, C compile mı, linker mı?
3. Merged DTS oluşmuş mu?
4. `.config` içinde beklenen config var mı?
5. `devicetree_generated.h` içinde beklenen macro var mı?

Mini görev:

- Bilerek küçük bir DTS hatası yapmadan önce dosyayı yedekleme/commit ile güvene
  al.
- Hata mesajının hangi katmana ait olduğunu ayır.

## 9. Mevcut Projeye Bağlama

Bu repo için takip edilecek pratik sıra:

1. `app.overlay` içindeki ADC node'larını oku.
2. Aynı node'ları `build/zephyr/zephyr.dts` içinde bul.
3. `DT_PATH(zephyr_user)` ve `ADC_DT_SPEC_GET_BY_IDX(...)` zincirini takip et.
4. `prj.conf` içindeki `CONFIG_ADC=y` ile ADC driver/subsystem ilişkisini ayır.
5. LED tarafına geri dönüp alias üzerinden blink mantığını tekrar kur.
6. Sonra ayrı bir custom board denemesi aç.

## İlk Ders: Başlangıç Kontrol Listesi

Başlamak için ilk oturumda şunları yapacağız:

- [ ] Build ortamında `west` erişilebilir mi kontrol et.
- [ ] Projeyi temiz build et.
- [ ] `build/zephyr/zephyr.dts` dosyasını aç.
- [ ] `app.overlay` değişikliklerinin merged DTS'e girdiğini doğrula.
- [ ] `devicetree_generated.h` içinde ADC ve LED macro'larından birini bul.
- [ ] `.config` içinde `CONFIG_GPIO` ve `CONFIG_ADC` değerlerini kontrol et.
- [ ] Bir node için alias -> node label -> property zincirini elle yaz.

Bu ilk dersin çıktısı:

```text
"Ben DTS yazınca Zephyr bunu hangi dosyalara dönüştürüyor ve C kodu o bilgiyi
nasıl kullanıyor?" sorusuna cevap verebilmek.
```

