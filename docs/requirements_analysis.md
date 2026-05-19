# Gilded Rose 요구사항 분석

## 1. 아이템 타입별 비즈니스 규칙

| 아이템 타입 | 식별 문자열 | `sellIn` 변경 | 기본 `quality` 변경 | 판매 기한 경과 후 규칙 | `quality` 경계 |
| --- | --- | --- | --- | --- | --- |
| Normal | `Aged Brie`, `Backstage passes to a TAFKAL80ETC concert`, `Sulfuras, Hand of Ragnaros`, `Conjured` 계열이 아닌 일반 이름 | 매일 1 감소 | 매일 1 감소 | `sellIn` 감소 후 0 미만이면 추가로 1 감소하여 총 2 감소 | 0 미만 불가, 50 초과 불가 |
| Aged Brie | `Aged Brie` | 매일 1 감소 | 매일 1 증가 | `sellIn` 감소 후 0 미만이면 추가로 1 증가하여 총 2 증가 | 50 초과 불가 |
| Backstage Pass | `Backstage passes to a TAFKAL80ETC concert` | 매일 1 감소 | `sellIn`이 11일 이상이면 1 증가, 10일 이하이면 2 증가, 5일 이하이면 3 증가 | 콘서트가 지난 뒤에는 `quality`가 0 | 50 초과 불가, 콘서트 이후에는 0 |
| Sulfuras | `Sulfuras, Hand of Ragnaros` | 변경 없음 | 변경 없음 | 판매 기한 경과 개념을 적용하지 않음 | 예외적으로 `quality` 80 유지 |
| Conjured | 이름에 `Conjured` 요구사항을 나타내는 문자열 | 매일 1 감소 | Normal 아이템의 2배 속도로 감소 | 판매 기한 경과 후에도 Normal의 2배 속도를 적용하여 총 4 감소 | 0 미만 불가, 50 초과 불가 |

## 2. 문자열 비교와 분기 시 주의점

1. 현재 C++ 구현은 `std::string`의 정확한 값 비교(`==`, `!=`)에 의존한다. 아이템 이름에 공백, 쉼표, 대소문자, 철자가 조금만 달라도 다른 아이템 타입으로 처리된다.

2. `Sulfuras, Hand of Ragnaros`와 `Backstage passes to a TAFKAL80ETC concert`는 긴 고정 문자열이므로 하드코딩을 반복하면 오타 위험이 크다. C++17 기준에서는 `constexpr const char*` 또는 `const std::string` 상수로 분리하는 것이 안전하다.

3. `Conjured`는 신규 요구사항상 카테고리처럼 표현된다. 이름이 정확히 `Conjured`인 아이템만 처리할지, `Conjured Mana Cake`처럼 이름에 `Conjured`가 포함된 아이템까지 처리할지 명세를 코드에서 명확히 해야 한다.

4. `std::string::find("Conjured")`를 사용할 경우 반환값은 위치 인덱스 또는 `std::string::npos`이다. `find(...) == 0`은 이름이 `Conjured`로 시작하는 경우만 참이고, `find(...) != std::string::npos`는 이름 어딘가에 포함된 경우 모두 참이다.

5. `find("Conjured") != std::string::npos`를 넓게 쓰면 `Not Conjured Item` 같은 이름도 Conjured로 오분류될 수 있다. 테스트에서 허용할 이름 패턴을 먼저 고정해야 한다.

6. 분기 순서는 중요하다. `Sulfuras`는 어떤 감소 로직보다 먼저 예외 처리되어야 하며, `Backstage Pass`와 `Aged Brie`는 Normal 감소 로직으로 흘러가면 안 된다.

## 3. 예외 및 경계값 조건

1. `quality`는 일반적으로 0 이상 50 이하로 유지되어야 한다.

2. `quality == 0`인 감소형 아이템은 더 이상 감소하지 않아야 한다. Normal과 Conjured 모두 0 미만으로 내려가면 안 된다.

3. `quality == 50`인 증가형 아이템은 더 이상 증가하지 않아야 한다. Aged Brie와 Backstage Pass 모두 50을 초과하면 안 된다.

4. `sellIn == 0`은 업데이트 전 마지막 판매 가능일로 취급된다. 하루 업데이트 후 `sellIn`이 `-1`이 되며, 판매 기한 경과 규칙이 적용된다.

5. `sellIn == -1` 또는 더 작은 음수 값은 유효한 입력이다. 음수 `sellIn`을 거부하거나 0으로 보정하면 안 된다.

6. Normal 아이템은 판매 기한 경과 후 하루에 `quality`가 2 감소한다.

7. Aged Brie는 판매 기한 경과 후 하루에 `quality`가 2 증가하되 50을 초과하지 않는다.

8. Backstage Pass는 업데이트 후 판매 기한이 지난 상태가 되면 `quality`가 0이 된다.

9. Sulfuras는 예외적으로 `quality`가 80이며, `sellIn`과 `quality`가 모두 변하지 않는다.

10. Sulfuras에는 일반적인 `quality` 상한 50을 적용하지 않는다. 50으로 clamp하면 요구사항 위반이다.

## 4. Conjured 신규 요구사항

1. Conjured 아이템은 Normal 아이템과 동일하게 매일 `sellIn`이 1 감소한다.

2. Conjured 아이템의 `quality` 감소량은 Normal 아이템의 2배이다.

3. 판매 기한이 지나지 않은 Conjured 아이템은 하루에 `quality`가 2 감소한다.

4. 판매 기한이 지난 Conjured 아이템은 Normal의 판매 기한 경과 감소량 2의 2배를 적용하여 하루에 `quality`가 4 감소한다.

5. Conjured 아이템도 `quality`는 0 미만으로 내려가면 안 된다.

6. Conjured 아이템은 증가형 아이템이 아니므로 `quality` 50 상한은 주로 초기값 검증과 향후 불변식 확인 관점에서 다룬다.

7. 구현 시 `Conjured` 식별 방식은 테스트와 함께 고정해야 한다. 권장 기준은 `name.find("Conjured") != std::string::npos`를 사용할지, 특정 정확한 이름만 `==`로 비교할지 명확히 선택하는 것이다.

## 5. Google Test 기준 테스트 시나리오

1. Normal 아이템은 하루가 지나면 `sellIn`이 1 감소하고 `quality`가 1 감소한다.

2. Normal 아이템의 `sellIn == 0`이면 업데이트 후 `sellIn == -1`이 되고 `quality`가 총 2 감소한다.

3. Normal 아이템의 `sellIn == -1`이면 업데이트 후에도 판매 기한 경과 규칙으로 `quality`가 2 감소한다.

4. Normal 아이템의 `quality == 0`이면 업데이트 후에도 `quality`는 0이다.

5. Aged Brie는 하루가 지나면 `sellIn`이 1 감소하고 `quality`가 1 증가한다.

6. Aged Brie의 `sellIn == 0`이면 업데이트 후 `quality`가 총 2 증가한다.

7. Aged Brie의 `quality == 50`이면 업데이트 후에도 50을 초과하지 않는다.

8. Backstage Pass의 `sellIn >= 11`이면 `quality`가 1 증가한다.

9. Backstage Pass의 `sellIn == 10`이면 `quality`가 2 증가한다.

10. Backstage Pass의 `sellIn == 5`이면 `quality`가 3 증가한다.

11. Backstage Pass의 `sellIn == 0`이면 업데이트 후 `quality`가 0이 된다.

12. Backstage Pass의 `quality == 49` 또는 `quality == 50` 근처에서도 50을 초과하지 않는다.

13. Sulfuras는 하루가 지나도 `sellIn`이 변하지 않는다.

14. Sulfuras는 하루가 지나도 `quality`가 80으로 유지된다.

15. Sulfuras의 `sellIn == 0` 또는 음수여도 `sellIn`과 `quality`가 변하지 않는다.

16. Conjured 아이템은 하루가 지나면 `sellIn`이 1 감소하고 `quality`가 2 감소한다.

17. Conjured 아이템의 `sellIn == 0`이면 업데이트 후 `quality`가 총 4 감소한다.

18. Conjured 아이템의 `sellIn == -1`이면 판매 기한 경과 규칙으로 `quality`가 4 감소한다.

19. Conjured 아이템의 `quality`가 1, 2, 3처럼 감소량보다 작아도 업데이트 후 0 미만이 되지 않는다.

20. Conjured 식별 문자열 정책에 따라 `Conjured Mana Cake` 같은 이름이 Conjured로 처리되는지 검증한다.

21. 알 수 없는 이름의 아이템은 Normal 아이템 규칙으로 처리된다.

22. 여러 아이템이 벡터에 함께 들어 있어도 각 아이템의 규칙이 독립적으로 적용된다.
