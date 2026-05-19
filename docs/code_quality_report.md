# updateQuality() 코드 품질 분석 보고서

분석 대상: `cpp/src/GildedRose.cpp`의 `GildedRose::updateQuality()`

우선순위 기준: `1`이 가장 높고, `5`가 가장 낮다.

| 문제점 | 위반 원칙/스멜 | 영향 | 개선 방향 | 우선순위 |
| --- | --- | --- | --- | --- |
| `updateQuality()`가 아이템 분류, 품질 증감, 판매 기한 감소, 만료 후 처리, 경계값 보정까지 모두 직접 수행한다. | SRP 위반, Long Method | 하나의 메서드가 여러 변경 이유를 갖는다. 신규 아이템 규칙이나 경계값 정책 변경 시 같은 메서드를 계속 수정해야 하며 회귀 위험이 커진다. | 아이템별 품질 갱신 규칙을 `NormalPolicy`, `AgedBriePolicy`, `BackstagePassPolicy`, `SulfurasPolicy` 같은 작은 함수 또는 전략 클래스로 분리한다. `updateQuality()`는 순회와 정책 호출만 담당하게 한다. | 1 |
| 아이템 타입이 문자열 비교 조건문에 하드코딩되어 있고, 신규 타입 추가 시 기존 조건문을 수정해야 한다. | OCP 위반, 조건문 복잡도 | `Conjured` 같은 새 규칙을 추가하려면 기존 `if/else` 체인을 직접 변경해야 한다. 기존 규칙의 분기 순서를 건드리게 되어 사이드 이펙트가 발생하기 쉽다. | C++17 기준으로 전략 패턴을 적용해 이름 기반 정책 선택기와 정책 실행을 분리한다. 간단한 규모라면 `std::unordered_map<std::string_view, Rule>` 또는 판별 함수 테이블로 시작할 수 있다. | 1 |
| `Aged Brie`, `Backstage passes to a TAFKAL80ETC concert`, `Sulfuras, Hand of Ragnaros` 문자열이 여러 조건문에 반복된다. | Duplicated Code, Primitive Obsession | 오타에 취약하고, 이름 변경이나 정책 분리 시 수정 지점이 늘어난다. 긴 문자열이 비즈니스 규칙보다 조건식 노이즈를 키운다. | `constexpr std::string_view` 상수로 아이템 이름을 분리한다. 예: `kAgedBrieName`, `kBackstagePassName`, `kSulfurasName`. | 2 |
| `0`, `50`, `11`, `6` 같은 숫자가 의미 없이 직접 등장한다. | Magic Number | `quality` 하한, 상한, Backstage Pass 임계일이 코드만 보고 즉시 드러나지 않는다. 특히 `11`, `6`은 실제 요구사항의 "10일 이하", "5일 이하"를 `< 11`, `< 6`으로 표현한 값이라 의도가 숨겨진다. | `kMinQuality = 0`, `kMaxQuality = 50`, `kBackstageDoubleIncreaseThreshold = 10`, `kBackstageTripleIncreaseThreshold = 5`처럼 도메인 언어로 상수화한다. 조건식도 `sellIn <= threshold` 형태로 바꾸면 요구사항과 코드가 맞아진다. | 2 |
| 품질 증감 전후에 `quality > 0`, `quality < 50` 검사가 여러 위치에 반복된다. | Duplicated Code, Shotgun Surgery | 경계값 정책을 바꾸거나 감소량을 조정할 때 모든 분기를 맞춰 수정해야 한다. 누락 시 `quality`가 0 미만 또는 50 초과가 될 수 있다. | `increaseQuality(Item&, int amount)`, `decreaseQuality(Item&, int amount)`, `clampQuality()` 같은 보조 함수를 만든다. `Sulfuras` 예외는 일반 clamp 대상에서 제외한다. | 2 |
| `items[i].quality = items[i].quality - items[i].quality`로 Backstage Pass 만료 후 품질을 0으로 만든다. | Intention-Revealing Name 부족, Obscure Code | 결과는 0이지만 의도가 명확하지 않아 읽는 사람이 산술 트릭을 해석해야 한다. 정적 분석이나 리뷰에서도 비즈니스 규칙이 바로 보이지 않는다. | `items[i].quality = kMinQuality` 또는 `expireBackstagePass(item)`처럼 의도를 직접 표현한다. | 3 |
| `items[i]` 접근이 메서드 전체에 반복된다. | Feature Envy에 가까운 저수준 반복, Readability Smell | 동일 인덱스 접근이 많아 조건문을 읽기 어렵고, 실수로 다른 인덱스나 필드를 건드릴 가능성이 커진다. | 루프 내부에서 `auto& item = items[i];` 또는 range-for `for (auto& item : items)`를 사용한다. | 3 |
| 정상 아이템과 만료 후 정상 아이템의 품질 감소 로직이 서로 떨어진 조건문에 중복되어 있다. | Duplicated Code, Temporal Coupling | "하루 감소"와 "판매 기한 경과 후 추가 감소"라는 규칙이 코드 흐름에 흩어져 있어 변경 시 누락 위험이 있다. | Normal 계열은 `degradationRate(item)`를 계산한 뒤 한 번에 감소시키거나, 정책별 `update(Item&)` 안에서 만료 전/후 감소량을 명시적으로 계산한다. | 3 |
| Backstage Pass의 품질 증가 규칙이 중첩 조건문으로 표현되어 있다. | 조건문 복잡도, Long Method | `quality < 50` 검사가 증가 단계마다 반복되고, 임계일 변경 시 중첩 구조를 다시 해석해야 한다. | `increase = sellIn <= 5 ? 3 : sellIn <= 10 ? 2 : 1`처럼 증가량을 먼저 계산한 뒤 한 번만 clamp한다. 또는 임계값 테이블을 사용한다. | 3 |
| `Sulfuras` 예외 처리가 여러 지점에서 부정 조건으로 반복된다. | OCP 위반, Duplicated Code, Guard Clause 부재 | 변경되지 않아야 하는 아이템이 일반 로직 사이에 섞여 있어, 새 분기 추가 시 실수로 `sellIn`이나 `quality`를 변경할 위험이 있다. | 루프 초기에 `if (isSulfuras(item)) continue;`로 예외를 빠르게 종료하거나, `SulfurasPolicy`를 no-op 전략으로 분리한다. | 2 |
| `updateQuality()`가 문자열 기반 타입 판별과 갱신 알고리즘을 동시에 포함한다. | SRP 위반, Primitive Obsession | 타입 판별 정책과 비즈니스 규칙이 결합되어 테스트 단위가 커진다. `Conjured`처럼 부분 문자열 매칭이 필요한 경우 기존 조건식이 더 복잡해진다. | `classifyItem(item)` 또는 `selectPolicy(item)`를 분리한다. C++17에서는 `enum class ItemKind`와 정책 함수 테이블, 또는 `std::variant<Normal, AgedBrie, BackstagePass, Sulfuras>` 기반 모델을 검토할 수 있다. | 2 |
| 현재 구조는 테스트 없이 대규모 구조 변경을 시도하면 회귀 가능성이 높다. | Legacy Code Smell, Characterization Test 부족 | 리팩토링 중 동작 보존 여부를 확인하기 어렵다. 특히 경계값 `quality == 0`, `quality == 50`, `sellIn == 0`, `sellIn == -1`에서 회귀가 발생하기 쉽다. | 리팩토링 전 Google Test로 현재 동작을 특성화한다. Normal, Aged Brie, Backstage Pass, Sulfuras, 신규 Conjured 후보 규칙을 경계값 중심으로 고정한 뒤 구조를 바꾼다. | 1 |

## 개선 방향 요약

1. 먼저 특성화 테스트를 보강해 현재 동작을 고정한다. `quality` 경계값, `sellIn` 만료 경계, `Sulfuras` 불변성, Backstage Pass 임계일을 우선 검증한다.
2. 숫자와 문자열을 도메인 상수로 분리한다. 이 단계는 동작 변경 없이 가독성과 리뷰 안정성을 즉시 높일 수 있다.
3. `increaseQuality()`와 `decreaseQuality()` 같은 작은 헬퍼를 도입해 `0..50` 경계 보정을 한 곳으로 모은다. `Sulfuras`의 예외 상한 `80`은 일반 품질 정책과 분리한다.
4. `updateQuality()` 내부에서 `auto& item` range-for를 사용하고, `Sulfuras`는 조기 반환 또는 no-op 정책으로 처리한다.
5. 최종적으로 아이템별 정책을 분리한다. 단순한 리팩토링 단계에서는 함수 테이블이나 `switch` 가능한 `ItemKind`가 충분하고, 아이템 타입이 계속 늘어난다면 전략 패턴을 적용한다.

## C++17 리팩토링 선택지

| 접근 | 적합한 상황 | 장점 | 주의점 |
| --- | --- | --- | --- |
| 작은 함수 추출 | 현재 구조를 안전하게 단계적으로 개선할 때 | 변경 폭이 작고 테스트와 함께 점진 적용하기 쉽다. | OCP 위반은 완전히 해소되지 않는다. |
| 전략 패턴 | 아이템 규칙이 계속 늘어나거나 각 규칙의 테스트 단위를 분리하고 싶을 때 | 신규 규칙 추가 시 기존 정책 코드를 덜 건드린다. `updateQuality()`의 책임이 명확해진다. | 클래스 수가 늘 수 있으므로 현재 규모에서는 과한 추상화가 될 수 있다. |
| 테이블 기반 규칙 | Backstage Pass처럼 임계값과 증가량이 데이터로 표현되는 규칙이 많을 때 | 매직 넘버를 데이터화하고 조건문 중첩을 줄인다. | 복잡한 예외 규칙은 테이블만으로 표현하면 오히려 읽기 어려울 수 있다. |
| `std::variant` 기반 모델 | 문자열 이름을 도메인 타입으로 변환한 뒤 타입 안전성을 높이고 싶을 때 | `std::visit`로 타입별 규칙을 명시할 수 있고 잘못된 조합을 줄일 수 있다. | 기존 `Item` 구조를 바꾸지 말아야 하는 제약이 있으므로 외부 래퍼나 분류 결과에만 제한적으로 적용해야 한다. |

## 권장 리팩토링 순서

| 순서 | 작업 | 이유 |
| --- | --- | --- |
| 1 | 특성화 테스트 보강 | 리팩토링 중 동작 보존을 확인할 안전망이 먼저 필요하다. |
| 2 | 문자열과 숫자 상수화 | 동작 변경 없이 가독성과 유지보수성을 빠르게 개선한다. |
| 3 | 품질 증감 헬퍼 추출 | 중복된 경계값 검사와 품질 변경 로직을 한곳으로 모은다. |
| 4 | 아이템별 업데이트 함수 분리 | `updateQuality()`의 Long Method와 SRP 위반을 실질적으로 줄인다. |
| 5 | 전략 패턴 또는 테이블 기반 구조 검토 | 신규 아이템 확장 가능성이 확인된 뒤 OCP를 본격적으로 개선한다. |
