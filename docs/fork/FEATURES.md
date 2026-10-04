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

## プレイヤーだけのレベル上限 / Player-only level cap ##

プレイヤー（キャラクター）だけ、レベルの上限を 99 より上（最大 127）にできます。
悪魔の上限は今までどおり（ワールドの設定 `LevelCap`、標準 99）です。

- 上限は `constants.xml` で決めます。サーバーの再起動で変えられるので、
  「今は 110 まで、次の更新で 115 まで」のように少しずつ開放できます。
- 99 → 100 から先の必要な経験値も `constants.xml` に書きます（1 レベル 1 個、カンマ区切り）。
- 上限に届いたキャラクターは経験値が増えません（上限を上げると、そこから続きを貯められます）。
- 上限を下げても、すでに上限より上のキャラクターのレベルは下がりません（経験値が増えなくなるだけ）。
- 死んだときの経験値の減少は、上限の手前まで（今までは 99 未満）。
- アーツ（エキスパート）の上限ポイントは、レベル 100 以上でも 99 と同じ扱いです。
- GM コマンド `@levelup` も 127 まで指定できます（上限より上にはなりません）。
- イベントの条件 `LEVEL`（例: レベル 100 以上で入れる場所）は、そのまま 100 以上の値で使えます。
- クライアントのレベルは 1 バイト（符号付き）なので、127 より上にはできません。
  レベル 100 以上の表示は窓によって欄が足りないことがあります。

### 設定 ###

```xml
<!-- プレイヤーの上限（0 または行が無い = 悪魔と同じ上限） -->
<constant name="PLAYER_LEVEL_CAP">110</constant>
<!-- 99→100, 100→101, … の必要経験値（上限 - 99 個以上） -->
<constant name="PLAYER_LEVEL_XP">895842000000,990142000000,...</constant>
```

`PLAYER_LEVEL_CAP` が 127 を超える、または `PLAYER_LEVEL_XP` の数が足りない場合はサーバーが起動しません。

### 関係するソース ###

- `libhack/src/ServerConstants.h/.cpp`: `PLAYER_LEVEL_CAP`、`PLAYER_LEVEL_XP`（省略可能）
- `server/channel/src/CharacterManager.h/.cpp`: `GetLevelCap(character)`、`GetLevelXP(level)` と、
  経験値の加算・レベルアップ・死亡時の減少・アーツの上限
- `server/channel/src/EventManager.cpp`（悪魔クエストの経験値）、`ChatManager.cpp`（`@levelup`）

*English: `PLAYER_LEVEL_CAP` (up to 127) raises the level cap for player
characters only; demons keep the world level cap. `PLAYER_LEVEL_XP` lists the
experience needed for each level from 99 up. Both are read at startup, so the
cap can be raised step by step.*
