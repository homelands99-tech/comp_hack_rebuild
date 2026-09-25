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
