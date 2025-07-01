## How am I working with the maps?
I am using Tiled to create maps:
- [hometown-table-16-16.png](../Source/images/areas/hometown-table-16-16.png) is the source tileset image.
- [tileset.tsx](../others/tileset.tsx) is the tileset file that Tiled uses. It depends on the image above.
- [00_downtown_tiledmap.tmx](../others/00_downtown_tiledmap.tmx) is the map file. It depends on the tileset file above.
- [00_downtown_tiledmap.json](../others/00_downtown_tiledmap.json) is the map file converted to JSON format. This is the one that the game reads. Generated from the file above.
    - The json has two layers:
        - `0` layer: contains the tiles that are not walkable.
        - `1` layer: contains the spawn points for monsters. But could be used for other things in the future.