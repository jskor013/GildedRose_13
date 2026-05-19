# Gilded Rose 테스트 계획서

## 1. 목적과 범위

이 문서는 C++17 기반 `GildedRose::updateQuality()`의 동작을 Google Test로 검증하기 위한 단위 테스트 계획이다. 현재 테스트는 `cpp/test/GildedRoseTest.cpp`의 placeholder 수준이므로, 리팩토링 또는 신규 `Conjured` 규칙 구현 전에 비즈니스 규칙을 `TEST_F` 기반 테스트로 고정하는 것을 1차 목표로 한다.

테스트 대상은 다음 파일과 인터페이스를 기준으로 한다.

- 대상 구현: `cpp/src/GildedRose.cpp`
- 공개 인터페이스: `GildedRose::updateQuality()`
- 테스트 파일: `cpp/test/GildedRoseTest.cpp`
- 빌드/테스트: `cpp/CMakeLists.txt`, `gilded_rose_test`

## 2. 테스트 전략

### 2.1 TEST_F 기반 구조

`TEST_F`를 기본 단위 테스트 스타일로 사용한다. 공통 fixture는 아이템 생성, 하루 업데이트 실행, 결과 검증을 단순화하되, 각 테스트는 하나의 비즈니스 규칙만 검증한다.

권장 fixture 예시:

- `GildedRoseTest`: 공통 헬퍼 제공
  - `UpdateOne(name, sellIn, quality)`로 단일 아이템 업데이트 결과 반환
  - `UpdateItems(items)`로 여러 아이템 독립 업데이트 검증
  - `ExpectItem(item, name, sellIn, quality)`로 Given-When-Then 의도를 명확화

테스트명은 규칙이 드러나도록 작성한다.

- `NormalItem_DecreasesSellInAndQualityByOne`
- `AgedBrie_IncreasesQualityTwiceAfterSellDate`
- `BackstagePass_DropsQualityToZeroAfterConcert`
- `Sulfuras_DoesNotChangeSellInOrQuality`
- `Conjured_DegradesTwiceAsFastAsNormalItem`

### 2.2 우선순위

| 우선순위 | 범위 | 목적 |
| --- | --- | --- |
| P0 | `Sulfuras` 불변성, `quality` 하한/상한, `sellIn == 0/-1` 만료 경계 | 치명적 회귀 방지 |
| P1 | Normal, Aged Brie, Backstage Pass의 대표 규칙 | 현재 명세의 핵심 동작 고정 |
| P1 | Backstage Pass 임계값 `sellIn == 10`, `5`, `0` | 조건문 경계 회귀 방지 |
| P1 | Conjured 신규 규칙 | 확장 요구사항 검증 |
| P2 | 여러 아이템 동시 업데이트 | 벡터 순회와 아이템 간 독립성 검증 |
| P2 | 이름 문자열 오타/유사 이름 처리 | 문자열 기반 분기 정책 확인 |
| P3 | 장기 시뮬레이션 및 조합 테스트 | 누적 업데이트 안정성 보강 |

## 3. 단위 테스트 범위

### 3.1 Normal Item

- 일반 아이템은 하루가 지나면 `sellIn`이 1 감소하고 `quality`가 1 감소한다.
- `sellIn == 0`이면 업데이트 후 `sellIn == -1`이 되고 `quality`는 총 2 감소한다.
- `sellIn == -1`이면 이미 판매 기한이 지난 상태로 보고 `quality`는 2 감소한다.
- `quality == 0`이면 더 이상 감소하지 않는다.
- 알 수 없는 이름의 아이템은 Normal 규칙으로 처리한다.

### 3.2 Aged Brie

- 하루가 지나면 `sellIn`이 1 감소하고 `quality`가 1 증가한다.
- `sellIn == 0`이면 업데이트 후 판매 기한 경과 규칙으로 `quality`가 총 2 증가한다.
- `sellIn == -1`에서도 하루에 2 증가한다.
- `quality == 49`에서는 50까지만 증가한다.
- `quality == 50`에서는 50을 초과하지 않는다.

### 3.3 Backstage Pass

- `sellIn >= 11`이면 `quality`가 1 증가한다.
- `sellIn == 10`부터 `quality`가 2 증가한다.
- `sellIn == 5`부터 `quality`가 3 증가한다.
- `sellIn == 0`이면 업데이트 후 콘서트가 지난 상태가 되어 `quality`는 0이 된다.
- `sellIn == -1`이면 이미 콘서트가 지난 상태로 보고 `quality`는 0이다.
- 증가 구간에서도 `quality`는 50을 초과하지 않는다.

### 3.4 Sulfuras

- `sellIn`은 어떤 값에서도 변하지 않는다.
- `quality`는 80으로 유지된다.
- 일반 `quality` 상한 50을 적용하지 않는다.
- `sellIn == 0`, `sellIn == -1`, `quality == 80` 조합을 반드시 포함한다.

### 3.5 Conjured

- Conjured 아이템은 하루가 지나면 `sellIn`이 1 감소한다.
- 판매 기한 전에는 `quality`가 2 감소한다.
- `sellIn == 0` 또는 `sellIn == -1`이면 `quality`가 4 감소한다.
- `quality`가 감소량보다 작은 `1`, `2`, `3`인 경우에도 0 미만으로 내려가지 않는다.
- 이름 식별 정책을 테스트로 고정한다.
  - 권장: `Conjured Mana Cake`처럼 `Conjured`를 포함한 이름을 Conjured로 처리할지 명확히 결정
  - 반례: `Not Conjured Item` 같은 이름을 어떻게 처리할지도 정책화 필요

## 4. 경계값 케이스 목록

아래 경계값은 아이템 타입별로 가능한 조합을 우선 테스트한다.

| 경계 | Normal | Aged Brie | Backstage Pass | Sulfuras | Conjured |
| --- | --- | --- | --- | --- | --- |
| `quality == 0` | 0 유지 | 1 또는 2로 증가 | 구간별 증가 또는 만료 시 0 | 일반 대상 아님 | 0 유지 |
| `quality == 1` | 0으로 감소 | 2 또는 3으로 증가 | 구간별 증가 또는 만료 시 0 | 일반 대상 아님 | 감소량 초과 시 0으로 clamp |
| `quality == 49` | 48 또는 47로 감소 | 최대 50 | 최대 50 | 일반 대상 아님 | 47 또는 45로 감소 |
| `quality == 50` | 49 또는 48로 감소 | 50 유지 | 50 유지 또는 만료 시 0 | 일반 대상 아님 | 48 또는 46으로 감소 |
| `sellIn == 0` | 업데이트 후 `-1`, 품질 2 감소 | 업데이트 후 `-1`, 품질 2 증가 | 업데이트 후 `-1`, 품질 0 | 변화 없음 | 업데이트 후 `-1`, 품질 4 감소 |
| `sellIn == -1` | 업데이트 후 `-2`, 품질 2 감소 | 업데이트 후 `-2`, 품질 2 증가 | 업데이트 후 `-2`, 품질 0 | 변화 없음 | 업데이트 후 `-2`, 품질 4 감소 |

세부 테스트 케이스:

- Normal: `(sellIn, quality) = (1, 1), (0, 1), (0, 2), (-1, 1), (-1, 50)`
- Aged Brie: `(1, 49), (1, 50), (0, 48), (0, 49), (-1, 49)`
- Backstage Pass: `(11, 49), (10, 49), (5, 48), (5, 49), (0, 50), (-1, 50)`
- Sulfuras: `(0, 80), (-1, 80), (10, 80)`
- Conjured: `(1, 1), (1, 2), (0, 3), (0, 4), (-1, 3), (-1, 50)`

## 5. 예외 및 특이 케이스

- `Sulfuras`는 유일하게 `quality == 80`을 허용하며, 일반 품질 상한 50의 예외이다.
- 음수 `sellIn`은 유효한 입력이다. 테스트는 음수 값을 거부하거나 0으로 보정하지 않는지 확인해야 한다.
- `Backstage Pass`는 업데이트 전 `sellIn == 0`인 경우, 증가 후 clamp가 아니라 최종 `quality == 0`이 우선이다.
- `quality`는 일반 아이템에서 0 미만 또는 50 초과가 되면 안 된다.
- 초기 입력이 명세 밖의 값인 경우의 정책은 별도로 합의한다.
  - 예: Normal `quality == 51`, Aged Brie `quality == -1`, Sulfuras `quality != 80`
  - 현재 계획의 기본 범위는 정상 입력과 경계 입력의 업데이트 결과 검증이다.
- 이름 비교는 정확한 문자열 기반이므로 오타, 대소문자 차이, 앞뒤 공백은 Normal로 처리될 수 있다.
- 여러 아이템을 한 벡터에 넣어 업데이트할 때 한 아이템의 규칙이 다른 아이템 결과에 영향을 주지 않아야 한다.

## 6. 커버리지 목표

### 6.1 목표

- 라인 커버리지: 90% 이상
- 브랜치 커버리지: 90% 이상
- 함수 커버리지: 100%
- 핵심 비즈니스 분기 커버리지: 100%
  - Normal
  - Aged Brie
  - Backstage Pass
  - Sulfuras
  - Conjured
  - `quality` 하한/상한
  - `sellIn` 만료 전/당일/이후

단순 라인 커버리지보다 분기 커버리지를 더 중요하게 본다. `updateQuality()`는 중첩 조건문이 많기 때문에 라인 90%를 달성해도 경계 분기가 누락될 수 있다.

### 6.2 gcov/lcov 측정 절차

GCC 또는 MinGW 환경에서 다음 절차를 사용한다.

```bash
cd cpp
cmake -B build-coverage -DCMAKE_BUILD_TYPE=Debug \
  -DCMAKE_CXX_FLAGS="--coverage -O0 -g" \
  -DCMAKE_EXE_LINKER_FLAGS="--coverage"
cmake --build build-coverage
ctest --test-dir build-coverage --output-on-failure
lcov --capture --directory build-coverage --output-file coverage.info
lcov --remove coverage.info "*/_deps/*" "*/test/*" --output-file coverage.filtered.info
genhtml coverage.filtered.info --output-directory coverage-html
```

Windows PowerShell에서는 줄바꿈 대신 한 줄로 실행하거나 백틱을 사용한다.

```powershell
cmake -B build-coverage -DCMAKE_BUILD_TYPE=Debug -DCMAKE_CXX_FLAGS="--coverage -O0 -g" -DCMAKE_EXE_LINKER_FLAGS="--coverage"
cmake --build build-coverage
ctest --test-dir build-coverage --output-on-failure
lcov --capture --directory build-coverage --output-file coverage.info
lcov --remove coverage.info "*/_deps/*" "*/test/*" --output-file coverage.filtered.info
genhtml coverage.filtered.info --output-directory coverage-html
```

### 6.3 개선 전략

1. 먼저 P0/P1 테스트를 작성해 핵심 분기를 채운다.
2. `lcov --list coverage.filtered.info` 또는 HTML 리포트에서 누락 라인을 확인한다.
3. 누락 라인이 비즈니스 의미가 있는 분기인지 확인한다.
4. 의미 있는 분기라면 경계값 테스트를 추가한다.
5. 의미 없는 방어 분기나 도달 불가능 코드가 있다면 코드 단순화 후보로 기록한다.
6. 커버리지 개선과 리팩토링은 분리해서 진행한다. 먼저 테스트를 통과시킨 뒤 구조 개선을 수행한다.

우선적으로 보강할 누락 가능성이 높은 분기:

- `quality == 0`에서 감소 로직을 건너뛰는 분기
- `quality == 50`에서 증가 로직을 건너뛰는 분기
- Backstage Pass `sellIn < 11`, `sellIn < 6`, 만료 후 0 처리 분기
- Sulfuras가 `sellIn` 감소와 `quality` 감소를 모두 건너뛰는 분기
- Conjured가 추가될 경우 일반 아이템과 다른 감소량을 적용하는 분기

## 7. 실행 기준

기본 테스트 실행:

```bash
cd cpp
cmake -B build
cmake --build build
ctest --test-dir build --output-on-failure
```

완료 기준:

- placeholder 테스트를 모두 실제 `TEST_F` 테스트로 대체한다.
- P0/P1 테스트가 모두 구현되어 통과한다.
- 라인/브랜치 커버리지 90% 이상을 달성한다.
- `Sulfuras`, `Backstage Pass` 만료, `quality` 0/50, `sellIn` 0/-1 경계가 누락 없이 검증된다.
- Conjured 요구사항을 구현하는 경우, 구현 전 실패하는 테스트를 먼저 추가하고 구현 후 통과시킨다.
