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

## サブ召喚（2 体目の仲魔・ミニオン） / Sub summon (second demon, minions) ##

COMP の仲魔をもう 1 体、AI で動く味方として連れて歩けます（2 体目）。
アイテムやスキルで決まった型の悪魔（ミニオン）を呼ぶこともできます。

- 2 体目: NPC の会話で「今召喚している仲魔」を登録し、機能番号 `SKILL_SUB_DEMON` のスキルで呼ぶ・戻す。
  キャラごとに「1 体目を召喚したら自動で呼ぶ」を ON にできる（NPC の会話で切り替え）。
- 2 体目はプレイヤーの後ろを付いて歩き、プレイヤーの敵と戦い、回復・補助・蘇生をする。
  行動の数値は AI スクリプト `scripts/AI/ai_subdemon.nut`（名前 `subDemon`）の `settings` で決める。
- 2 体目が倒されると COMP の仲魔の HP は 0 になる。格納・削除・召喚したときは自動で戻る。
- ミニオン: `data/minions.xml` の型（置物の補助役 SUPPORT / 戦う FIGHTER）を、
  機能番号 `SKILL_MINION` のスキル（特別な値 1 番目 = 型の番号）やアイテムで呼ぶ。
- 2 体目・ミニオンの与えたダメージはプレイヤーのものとして数える。
- データベース: `Character` に `SubDemon`（登録した仲魔）と `SubDemonAuto` を追加（world の起動時に列が足される）。

### 設定 ###

```xml
<constant name="SUB_DEMON_ENABLED">1</constant>   <!-- 2 体目を使う -->
<constant name="SKILL_SUB_DEMON">1600</constant>  <!-- 2 体目を呼ぶスキルの機能番号 -->
<constant name="MINION_ENABLED">1</constant>      <!-- ミニオンを使う -->
<constant name="SKILL_MINION">1601</constant>     <!-- ミニオンを呼ぶスキルの機能番号 -->
<constant name="SUB_MINION_TOGETHER">0</constant> <!-- 1 = 2 体目とミニオンを同時に出せる -->
```

どれも省略可能で、省略または `0` なら無効です。

### 関係するソース ###

- `libhack/schema/character.xml`: `SubDemon`、`SubDemonAuto`
- `libhack/schema/demon.xml`: `MinionData`（`data/minions.xml`）
- `server/channel/src/CharacterManager.h/.cpp`: `RegisterSubDemon()`（スクリプトから使う）、`ToggleSubDemon()`、
  `SummonSubDemon()`、`DismissSubDemon()`、`AutoSummonSubDemon()`、`SummonMinion()` など
- `server/channel/src/AIManager.cpp`: 付いて歩く・回復・補助・蘇生（`CompanionFollow()`、`PrepareCompanionSkill()`）
- `server/channel/src/SkillManager.cpp`: スキルの機能 `SubDemon()`・`Minion()`
- GM コマンド: `@sub`、`@minion`

*English: Players can register a COMP demon at an NPC (event script calling
`CharacterManager::RegisterSubDemon`) and bring it out as an AI ally with the
`SKILL_SUB_DEMON` skill. Minion types in `data/minions.xml` can be summoned
with the `SKILL_MINION` skill. Enable with `SUB_DEMON_ENABLED` / `MINION_ENABLED`.*

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
