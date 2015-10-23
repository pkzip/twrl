# twrl

TradewarsRL 2015
A 7drl by Mad Sorcerer

http://madsorcerer.com/tradewarsrl

This game takes classic multi-player BBS Tradewars and re-makes it as a single-
player roguelike.  You start in Sector 1 of the Federation.  Your quest is to
journey deep into space, track down the Ferrengi Mothership "Yendor" and
destroy it once and for all.

Features include:

- Turn-based game
- Vintage 16-colour scrolling console ASCII interface
  - Windows version uses native console colour control API
  - UNIX version uses ANSI control sequences (requires suitable terminal)
- Randomly generated universe, different with each game
  - connected sectors as in Hunt the Wumpus (1972)
    [ http://en.wikipedia.org/wiki/Hunt_the_Wumpus ]
  - sectors are grouped in regions (similar to Rogue levels)
- Permadeath
- Starbases for repairs and upgrading your ship
  - Upgrade your phasers and shields - if you have enough credits!
- Ports for trading
  - 3 commodities: ore, organics, and equipment
  - Limited amounts can be bought/sold before the port is "used up"
- 9 types of Ferrengi ships to combat
  - Re-spawns
  - Auto-salvage commodities from debris
- Auto-navigation / auto-pilot
  - Finds shortest path back to known sectors
  - Auto-pilot disengages in the event of attack

 How to play:

 - Launch the game (twrl or twrl.exe).
 - You start in sector 1.
 - The display shows which sector numbers can be warped to.
   - Sectors that are not yet visitted are shown with () around them.
   - Sectors which are located in different regions of space are shown
     with [] around them.
 - Enter a sector number and press <Enter> to move to it.
   - If the sector is adjacent, you immediately warp there.
   - For other sectors, if you have seen the warp lane, auto-pilot will engage.
 - Press ? for a list of other commands.
 - Use the <R>egional logs to see what you know about the current region.
   - This includes ports, starbases, and all warp lanes.
 - Use the <I>nventory to see what commodities you have in your holds, and
      how many credits you have available.
 - Use <T>actical reports to see your phaser and shield status.
 - Dock at ports and trade commodities to make money (credits).
   - Ports trade in 3 commodities: ore, organics, and equipment.
   - A given port will sell one and buy the other two.
   - There is a fixed spread between buy and sell prices (your profit).
   - For convenient references, commodities are always listed in this order.
   - For example: buy/sell/buy (or BSB in the regional log) means buys ore,
     sells organics, and buys equipment.
   - Ports have a limited number of commodities they will buy and sell.  The
     exact number is considered proprietary information by the vendors!
 - Watch out for Ferrengi and <A>ttack as required.
   - Deeper regions of space contain more powerful Ferrengi vessels.
   - If you win the battle, auto-salvage will engage and collect any
     sellable commodities from the debris into any empty holds you have.
   - Equipment will be preferred over organics, which are preferred over ore.
 - Spend credits at starbases to repair ship and upgrade shields and phasers
   before heading deeper into space.
   - All shield emitters must be repaired before upgrades can be performed.
   - Shield emitters will self-repair over time, but you risk encountering
     more Ferrengi before the repairs complete.
 - Defeat the Ferrengi Mothership "Yendor" and you win the game.
 - Enjoy!

