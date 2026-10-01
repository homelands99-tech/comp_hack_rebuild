# 追加した機能 / Added features #

## 原料タンク自動収納 / Automatic material tank storage ##

敵のドロップや宝箱からアイテムを拾ったとき、原料タンクに収納できる素材を
自動で原料タンクに入れます。

- タンクに全部入る素材は、インベントリが満杯でも拾えます。
- タンクの上限を超えた分は、今までどおりインベントリに入ります。
- 収納したときは、チャット欄に「原料タンクに ○○ を N 個収納しました。（計 M）」と表示されます。
- 原料タンク（貴重品 `VALUABLE_MATERIAL_TANK`）を持っていないキャラクターでは何も変わりません。
- 収納できるかどうかの判定は、手動でタンクに入れるとき（`MaterialInsert`）と同じです
  （`TankData.sbin` と `DisassemblyTriggerData.sbin` に登録された素材）。
- 対象は「拾ったとき」だけです。クエスト報酬・ショップ購入・トレードなどは対象外です。
- データベースの構造は変更しません。

### 設定 ###

`config/constants.xml` に次の行を追加すると有効になります。

```xml
<constant name="AUTO_MATERIAL_TANK">1</constant>
```

`0` または行が無い場合は無効です。

### 関係するソース ###

- `libhack/src/ServerConstants.h/.cpp`: `AUTO_MATERIAL_TANK`（省略可能）
- `libhack/src/DefinitionManager.h/.cpp`: `GetItemName()`（アイテム ID から名前）
- `server/channel/src/CharacterManager.h/.cpp`: `GetMaterialTankSpace()`、`StoreInMaterialTank()`
- `server/channel/src/packets/game/LootItem.cpp`: アイテムを拾う処理への組み込み

*English: When a character with the material tank loots items, materials that
can be stored in the tank go there automatically (up to the tank limit; the
rest goes to the inventory). Loot that fits in the tank can be picked up even
with a full inventory. Enable with `AUTO_MATERIAL_TANK` = 1 in
constants.xml.*

## デモンフォースのまとめ使い / Bulk demon force ##

悪魔にフォース用アイテムを使うとき、1回の使用で同じアイテムを複数個まとめて
消費します（クライアントの変更は不要です）。

### フォースの仕組み（元の動作） ###

- アイテム1個で、20種類の能力値フォース（HP最大・力・経験値など）に少しずつ
  ポイントが入ります。100,000 ポイントで能力値 +1 です（`DEMON_FORCE_PRECISION`）。
  上限はアイテムごとに `DevilBoostData` の結果欄で決まります。
- アイテム1個でベネフィットゲージが +1 されます。ゲージが 10・30・60、その後は
  100 ごと（`DevilBoostLotData`）になると、パッシブ（`DevilBoostExtraData` の特性）
  の候補が抽選で1つ出ます。候補を枠（最大8枠）に入れるか捨てるまで、次のアイテムは
  使えません。
- 特定のパッシブを直接枠に入れるアイテムもあります。

### まとめ使いの動作 ###

- 1回の使用で最大 N 個を続けて使います。次の場合はその時点で止まります。
  - パッシブの候補が出たとき（いつもの選択画面が出ます）
  - 能力値フォースがそのアイテムの上限に達したとき
  - 持っているアイテムが無くなったとき（同じアイテムの別の山からも使います）
- 枠に直接入れるアイテムは、今までどおり1個ずつです。
- ゲージだけを上げるアイテム（「契の玉・回」など）もまとめて使われ、パッシブ候補が出るか、
  アイテムが無くなるまで続きます。

### `@force` コマンド ###

有効なときは一般プレイヤーも使えます。

- `@force`: 召喚中の悪魔のゲージ、次のパッシブ候補までの残り個数、各フォースの値を表示
- `@force 個数`: 1回で使う個数を設定（1〜上限。ログアウトすると上限に戻ります）
- `@force max`: 上限に戻す

### 設定 ###

`config/constants.xml` に次の行を追加すると有効になります。数字は1回で使う上限です。

```xml
<constant name="DEMON_FORCE_BULK">100</constant>
```

`0`・`1` または行が無い場合は無効です（1個ずつ、`@force` は GM のみ）。

### 関係するソース ###

- `libhack/src/ServerConstants.h/.cpp`: `DEMON_FORCE_BULK`（省略可能）
- `server/channel/schema/clientstate.xml`: `DemonForceBulk`（`@force` で決めた個数、保存しない）
- `server/channel/src/packets/game/DemonForce.cpp`: まとめ使いの処理
- `server/channel/src/ChatManager.h/.cpp`: `@force`

*English: With `DEMON_FORCE_BULK` = N in constants.xml, one demon force use
consumes up to N of the same item, stopping when a new force stack effect
becomes pending, when nothing would be raised, or when the items run out.
`@force` (open to players while enabled) shows the gauge, uses left until the
next stack effect and the force values, and sets the per-use count.*
