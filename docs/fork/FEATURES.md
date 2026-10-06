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

## ガチャ画面の復活 / Gacha ##

クライアントに残っているガチャ画面を、COMP ショップの一覧から開けるようにします。
CP で引き、景品は宅配に届きます。ショップ XML の `Type` を `GACHA` にしたときだけ動き、
対応したクライアント（非公開）が必要です。GM コマンド `@gacha` やタブの条件で、表示・非表示と
景品の切り替えができます。

詳しくは [GACHA.md](GACHA.md) を見てください。

### 設定 ###

```xml
<constant name="GACHA_ENABLED">1</constant>
```

`0` または行が無い場合は無効です（`GACHA` のショップは一覧に出ません）。

*English: Restores the client's gacha window. Shops with `Type` `GACHA` are
listed in the COMP shop menu (needs a client that supports it, not public); drawing charges CP and
sends a weighted random prize to the post. Toggle with `@gacha` or tab
conditions. Enable with `GACHA_ENABLED` = 1 in constants.xml. See GACHA.md.*

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
- `@force effect off` / `on`: ゲージ満タン時の悪魔のエフェクトを出さない / 出す（ログアウトで「出す」に戻ります）。
  画面の画像表示と約4秒の待ち時間はクライアント側の演出なので変わりません

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

## 追加機能の文言（custom_messages） / Texts of the added features ##

追加した機能がプレイヤーに出す文言（チャット欄のメッセージ・GM コマンドの表示）は、
サーバーのデータ `datastore/data/custom_messages.xml` で変えられます（サーバーの再起動で反映）。

```xml
<objects>
    <object name="CustomMessage">
        <member name="ID">MATERIAL_TANK_STORED</member>
        <member name="Text">原料タンクに %1 を %2 個収納しました。（計 %3）</member>
        <member name="DefaultText">原料タンクに %1 を %2 個収納しました。（計 %3）</member>
        <member name="Note">説明（サーバーは読まない）</member>
    </object>
</objects>
```

- `Text` が文言です。`%1`・`%2` などには数や名前が入ります。
- ファイルが無い・その ID が無い・`Text` が空のときは、ソースに書いてある文言（`DefaultText` と同じ）を使います。
- `data/custom_messages/` フォルダに複数のファイルを置くこともできます（ファイルが 1 つも無いときだけ
  `data/custom_messages.xml` を読みます）。
- 追加する機能の文言は、ソースでは `ChannelServer::GetCustomMessage("ID", "文言")` で出します。

*English: Texts shown to players by the added features can be overridden in
`data/custom_messages.xml` (CustomMessage ID / Text). Without an entry the
default from the source is used.*
