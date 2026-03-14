# Part 1: Headband Construction & Sensor Wiring

**Estimated Time:** 2-3 Hours
**Difficulty:** Moderate - soldering small SMD pads requires patience

---

## Overview

In this part you will build the optical probe: the forehead-mounted assembly that holds two near-infrared LEDs and a photodiode at precise spacing. This is the most mechanically critical part of the entire project. If light leaks directly from an LED to the photodiode through the foam body, the device will not work - period.

---

## Materials Needed

| Item | Specification | Notes |
|:-----|:-------------|:------|
| Black EVA foam block | ~2cm x 4cm x 2cm minimum | Must be opaque black |
| 730nm LED | Cree JE2835AFR-N-0001A0000-N0000001 | Bare SMD 2835 package |
| 850nm LED | ams OSRAM SFH4253B | SMT PLCC-2 package |
| BPW34 Photodiode | Vishay | Through-hole, glass lens |
| 30 AWG stranded wire | 4 lengths, 8-10cm each | For LED pigtails |
| Shielded wire | 1 length, < 5cm | For photodiode connection |
| Black rubber washers | 2-3 pcs, 1/8" (3mm) inner diameter | For 730nm optical baffle |
| Superglue (CA glue) | - | Gel type preferred |
| Opaque electrical tape | Black | For light sealing |
| Elastic headband | ~2cm wide | Medical/sport type |
| Heat-shrink tubing | Small diameter for 30 AWG | For strain relief |
| Soft padding material | Medical gauze or soft fabric | For patient comfort |

### Tools Required

- Soldering iron with fine tip
- Flux pen or paste flux
- Wire strippers (30 AWG capable)
- Sharp craft knife or hole punch
- Heat gun or lighter (for heat-shrink)
- Ruler with mm markings
- Marker or pen for labeling

---

## Finished Probe - What You Are Building

Before starting, here is what the completed probe looks like from above (tissue-contact face up) and in cross-section:

```
  TOP VIEW (tissue-contact face - the side that touches the forehead)
  ================================================================

     +--------------------------------------------------+
     |                                                  |
     |    Photodiode (BPW34)          LEDs side-by-side |
     |                                                  |
     |       +-------+                +-----+-----+    |
     |       |  BPW  |                | 730 | 850 |    |
     |       |  34   |                | nm  | nm  |    |
     |       | (lens)|                |[WAS]|     |    |
     |       +-------+                +-----+-----+    |
     |                                                  |
     |       |<---------- 30mm c-to-c ---------->|      |
     |                                                  |
     +--------------------------------------------------+
              ^                        ^
              |                        |
         7mm hole                 LED pockets
```

```
  CROSS-SECTION (side view, cut through the center)
  ==================================================

     Wires exit to breadboard (back)
      ___|________________________________________|___
     |   |                                        |   |
     |  BPW34                                 LEDs    |  <-- Black EVA foam
     |  leads          foam body             leads    |      2cm x 4cm x 2cm
     |___|________________________________________|___|
         |                                        |
      [LENS]                              [FLUSH SURFACE]
         v                                        v
    ================================================== <-- Tissue-contact face
         |                                        |      (touches forehead)
         |<------------- 30mm ------------------>|
```

---

## Step 1: Prepare the LED Pigtail Wires (Both LEDs)

You will solder thin wires directly to each SMD LED before mounting them in the foam. This is the trickiest soldering in the entire project.

### 1.1 Cut and Strip Wires

- Cut four pieces of 30 AWG stranded wire, each 8-10cm long.
- Strip approximately 2mm of insulation from one end of each wire.
- Use two different wire colors if available (e.g., red for anode, black for cathode) to make polarity tracking easier.

### 1.2 Pre-Tin the Wires and LED Pads

- Apply a small amount of flux to the stripped wire ends and tin them with solder.
- Apply flux to the anode and cathode pads on both LEDs.
- Add a tiny dome of solder to each LED pad. Use minimal solder - these are small pads.

> **Tip:** Secure the LED to your work surface with a piece of tape or use helping hands. These parts are very small and will move if you don't anchor them.

### 1.3 Solder Pigtails to the 730nm LED (Cree JE2835AFR)

- Touch the pre-tinned wire to the pre-tinned anode pad and briefly apply the iron to reflow. The joint should take less than 2 seconds.
- Repeat for the cathode pad with the second wire.
- Inspect: each joint should be smooth and shiny. No solder bridges between pads.

### 1.4 Solder Pigtails to the 850nm LED (SFH4253B)

- Same procedure as Step 1.3. The SFH4253B is a PLCC-2 package, so the pads are slightly different in geometry but the same technique applies.

### 1.5 Add Heat-Shrink Strain Relief

- Slide a short piece of heat-shrink tubing over each solder joint (one per wire, four total).
- Shrink with a heat gun or carefully with a lighter held at a distance.
- This prevents the thin wires from breaking at the solder joint during handling.

### 1.6 Label Polarity

- Label each wire pair with tape flags: mark anode (+) and cathode (-) for both LEDs.
- Also label which LED is which (730nm vs 850nm). Mixing these up later will cause incorrect readings.

```
  FINISHED PIGTAILED LED (one of two)
  ====================================

    Red wire (+)       Black wire (-)
        \                 /
     [heat-shrink]   [heat-shrink]
          \             /
           \           /
        +---+----+----+---+
        |   Anode|Cath    |
        |   pad  |pad     |  <-- SMD LED (2835 or PLCC-2 package)
        |        |        |
        +--------+--------+
              ||
          (emitting surface -
           this side faces tissue)

  Make two of these: one for 730nm, one for 850nm.
  Label each pair clearly!
```

---

## Step 2: Prepare the Foam Base

### 2.1 Cut the Foam Block

- Cut the black EVA foam to dimensions: **2cm (height) x 4cm (length) x 2cm (width)**.
- Use a sharp craft knife for clean edges. Ragged foam edges create light leak paths.

### 2.2 Create the Photodiode Hole

- Punch or drill a **7mm diameter** hole cleanly through one end of the foam block.
- This hole should go all the way through the foam in the direction that will face the skin.
- The BPW34 should fit snugly - not loose, not so tight you have to force it.

### 2.3 Create the LED Pockets

- Measure exactly **30mm center-to-center** from the photodiode hole.
- At this 30mm mark, carve **two small adjacent pockets** sized to fit the bare LED bodies.
- These pockets should be deep enough that the LED emitting surface sits flush with the tissue-contact face of the foam (not recessed, not protruding).
- Include a small routing channel for the pigtail wires to exit out the back/top of the foam.

```
  FOAM BLOCK - LAYOUT (top view, looking down at tissue-contact face)
  ====================================================================

    |<---------------------- 4cm ---------------------->|

    +---+----------------------------------------------+---+  ---
    |   |                                              |   |   ^
    |   |   +-------+                  +-----+-----+   |   |   |
    |   |   | 7mm   |                  |pocket|pocket  |   |   2cm
    |   |   | hole  |                  | 730  | 850 |  |   |  width
    |   |   |       |                  |  nm  |  nm |  |   |   |
    |   |   +-------+                  +-----+-----+   |   |   v
    |   |                                              |   |  ---
    +---+----------------------------------------------+---+

            |<------------ 30mm c-to-c ----------->|

    BACK VIEW (looking at the wire-exit side)
    ==========================================

    +--------------------------------------------------+  ---
    |                                                  |   ^
    |   ====                           ====  ====      |   |
    |   wire                           wire  wire      |  2cm
    |   channel                        channels        | height
    |   (photodiode)                   (LED pigtails)  |   |
    |                                                  |   v
    +--------------------------------------------------+  ---
```

> **Why 30mm?** This source-detector separation determines the depth of tissue interrogated. At 30mm, the banana-shaped photon path reaches cortical brain tissue through the skull. Shorter distances sample only scalp; longer distances lose too much signal.

---

## Step 3: Build the 730nm Optical Baffle

The 730nm LED has a wide beam (~121 degrees). Without a baffle, stray light will travel through or across the foam surface directly to the photodiode, overwhelming the tissue signal.

### 3.1 Stack the Washers

- Take 2-3 black rubber washers (1/8" / 3mm inner diameter).
- Stack them and glue together with superglue. Let dry completely.

### 3.2 Attach Baffle to 730nm LED

- Apply superglue to the bottom of the washer stack.
- Carefully place the stack directly over the dome/emitting surface of the 730nm LED, centering the 3mm aperture over the LED.
- Hold firmly until glue sets.

```
  730nm LED WITH WASHER BAFFLE (cross-section)
  =============================================

                    | 3mm |
                    |<--->|
               _____|     |_____
              |  washer 3       |   ^
              |_____|     |_____|   |
              |  washer 2       |   | Baffle stack
              |_____|     |_____|   | (2-3 washers)
              |  washer 1       |   |
              |_____|     |_____|   v
              ======+=====+======
              |     LED dome    |  <-- 730nm SMD LED
              |    (emitter)    |
              +-wire-+-wire-+---+
                  +     -
                anode  cathode

    Light exits ONLY through the 3mm aperture,
    directed straight into tissue. Stray sideways
    light is blocked by the washer walls.


  850nm LED - NO BAFFLE NEEDED (cross-section)
  =============================================

              +-----------------+
              |     LED dome    |  <-- 850nm SMD LED
              |    (emitter)    |
              +-wire-+-wire-+---+
                  +     -
```

> **The 850nm LED does NOT need a baffle.** Only the 730nm requires this modification.

---

## Step 4: Mount the Emitters in the Foam

### 4.1 Insert LEDs

- Press the 730nm LED (with washer baffle attached) into its foam pocket.
- Press the 850nm LED into its adjacent pocket.
- The emitting surfaces must be **perfectly flush** with the tissue-contact face of the foam:
  - For the 730nm: the top of the washer stack should be flush.
  - For the 850nm: the top of the LED dome should be flush.

### 4.2 Secure and Route Wires

- Apply opaque adhesive (or black electrical tape) around the LED bodies inside the foam to lock them in place.
- Route the pigtail wires out through the back of the foam, away from the tissue-contact face.
- Keep the polarity labels visible and accessible.

---

## Step 5: Mount the Photodiode

### 5.1 Identify Photodiode Orientation

- The BPW34 has a convex glass lens on one side and a flat surface on the other.
- **Convex lens = active sensing surface** - this faces outward toward the tissue.
- **Flat side = cathode** - this faces into the foam body, away from tissue.

```
  BPW34 PHOTODIODE ORIENTATION
  ============================

  CORRECT (lens toward tissue):

       Foam body (back)
      ___________________
     |                   |
     |    flat side      |  <-- Cathode mark (flat edge or dot)
     |    (cathode)      |
     |   +-----------+   |
     |   |   BPW34   |   |
     |   |  _-----_  |   |
     |   | /  lens  \ |   |
     |   |( convex  )|   |
     |   | \       / |   |
     |   |  `-----'  |   |
     |   +-----------+   |
     |___________________|
              |
              v
       Tissue-contact face
       (touches forehead)

  SIDE VIEW:

        lead (-)     lead (+)
            \           /
     ........\........./..........  <-- Foam surface (back)
     :   +----+-------+----+    :
     :   |flat|       |    |    :  <-- BPW34 body
     :   |side| glass |lens|    :
     :   +----+--===--+----+    :
     .........|..===..|..........  <-- Foam surface (tissue-contact)
              | lens  |
              v  face v
         (toward forehead)
```

### 5.2 Insert and Wire the Photodiode

- Insert the BPW34 into the 7mm hole with the **lens facing the tissue-contact side**.
- Solder the **shielded wire** to the photodiode leads.

#### What is Shielded Wire?

Shielded wire (also called coaxial or screened cable) has two conductors: a **center conductor** (thin insulated wire in the middle) surrounded by a **shield** (braided metal mesh or foil wrapped around the outside, under the outer jacket). The shield blocks electromagnetic interference from reaching the center conductor.

```
  SHIELDED WIRE CROSS-SECTION
  ============================

        +-- Outer jacket (plastic)
        |   +-- Shield braid (woven metal mesh)
        |   |   +-- Inner insulation (plastic)
        |   |   |   +-- Center conductor (copper wire)
        |   |   |   |
        v   v   v   v
      +-+---+---+-+-+---+---+-+
      | |:::::::| | | |:::::::| |
      | |:::::::| | * | |:::::::| |
      | |:::::::| | | |:::::::| |
      +-+---+---+-+-+---+---+-+

  End view showing concentric layers:

         _______________
        /  :::::::::::  \       ::: = Shield braid
       / :: _________:: \
      | :: / _______ \ :: |
      | ::|/ inner  \|:: |
      | ::|| ins.   ||:: |     * = Center conductor
      | ::|| * wire ||:: |         (connects to CATHODE)
      | ::|\_______/|:: |
      | :: \_______/ :: |     Shield braid
       \ ::::::::::::: /       (connects to ANODE/GND)
        \______________/
```

**How to strip and solder shielded wire:**
1. Strip about 10mm of the outer jacket to expose the braided shield.
2. Carefully unbraid or push back the shield mesh, twisting it into a single bundle. Do not let loose shield strands touch the center conductor.
3. Strip about 3mm of the inner insulation from the center conductor.
4. Solder the connections:
   - **Shield (outer braid)** connects to the **anode** (the photodiode lead closest to the BPW34 lens/convex side).
   - **Center conductor (inner wire)** connects to the **cathode** (flat side lead).
5. Add heat-shrink or tape to each joint to prevent the shield and center conductor from shorting together.

- **Keep this wire under 5cm total length.** Longer wires pick up 60Hz mains noise and will corrupt your signal.

### 5.3 Secure the Photodiode

- Use a small amount of opaque adhesive or black electrical tape to hold the photodiode in place and seal any gaps around the hole.

---

## Step 6: Optical Isolation - The Most Critical Step

This step determines whether your device works or produces garbage data. Take your time.

### 6.1 Seal the LED Cavities

- Wrap additional black foam or multiple layers of opaque electrical tape around the back and sides of both LED housings.
- Pay special attention to the gap between the two LEDs and between the LEDs and the photodiode.

### 6.2 Verify the 730nm Baffle Seal

- The rubber washer stack must be perfectly seated against the foam with **no gaps** around its base.
- If you can see any daylight through the washer-to-foam interface when you hold it up to a light, seal it with black adhesive.

### 6.3 Seal the Photodiode Cavity

- Wrap the back side of the photodiode with opaque tape, leaving only the lens face exposed on the tissue-contact side.
- Ensure no light can enter the photodiode from the back or sides.

### 6.4 Seal the Foam Body

- Wrap the entire back and sides of the foam block (everything except the tissue-contact face) in opaque electrical tape.
- Any pin-hole or thin spot in the foam is a potential light leak path.

```
  OPTICAL ISOLATION - WHAT TO SEAL (cross-section)
  =================================================

       Seal ALL of this with opaque tape
       vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
      +###################################+
      #  wires out       wires out        #
      #  ====                 ====  ====  #  <-- Back: SEALED
      #  :                    :     :     #
      #  :BPW34          730nm: 850nm:    #
      #  :                    :     :     #
      +--+--------------------+-----+-----+
      |  [LENS]          [WASHER]  [LED]  |  <-- Tissue face: OPEN
      +-----------------------------------+      (only this side is exposed)

  LIGHT LEAK DANGER ZONES (top view):
  ====================================

      +---+----------------------------------------------+---+
      |   |                                              |   |
    S |   |   +-------+    DANGER ZONE     +-----+-----+ |   | S
    E |   |   | BPW34 |<--- check for --->| 730 | 850 | |   | E
    A |   |   |       |    light paths    |  nm |  nm | |   | A
    L |   |   +-------+  through foam OR  +-----+-----+ |   | L
      |   |              across surface                  |   |
      +---+----------------------------------------------+---+
                 ^                              ^
                 |                              |
            Must NOT see             Must NOT leak
          any LED light here       sideways through foam
```

> **Why this matters:** The photodiode is detecting nano-amps of photocurrent from light that has traveled through skull and brain tissue. Direct LED-to-photodiode light (even a tiny leak) produces micro-amps - a thousand times stronger. Even 0.1% leakage will completely overwhelm the tissue signal.

---

## Step 7: Headband Integration

### 7.1 Attach Foam to Headband

- Position the completed foam probe at the center of the elastic headband.
- Sew or glue the foam block to the headband. The tissue-contact face (with LEDs and photodiode flush) should face inward (toward the forehead when worn).

### 7.2 Add Comfort Padding

- Attach soft padding (medical gauze, soft fabric, or thin medical-grade foam) around the hard edges of the probe body.
- The probe will be pressed against a subject's forehead for 10+ minutes. Comfort matters.

### 7.3 Wire Management

- Route the pigtail wires (4 LED wires + shielded photodiode wire) along the headband toward one side.
- Secure with small pieces of tape or thread loops so wires don't dangle or snag.

```
  COMPLETED HEADBAND ASSEMBLY (front view, as worn)
  ==================================================

                        Elastic headband
           _______________|||_______________
          /               |||               \
         /                |||                \
        |      +----------+++----------+      |
        |      |       FOAM PROBE      |      |
        |      |   [BPW34]  [730][850] |      |
        |      +----------+++----------+      |
        |        soft     |||  soft           |
        |        padding  |||  padding        |
         \                |||                /
          \        wires--+++--->          /
           \_______________|_____________/
                           |
                           | 4 LED wires +
                           | 1 shielded photodiode wire
                           | (secured along band with tape)
                           |
                           v
                     To breadboard

  PLACEMENT ON FOREHEAD (side view):
  ===================================

                     Headband
                   ___/    \___
                  /   PROBE    \
                 / [===FOAM===] \
         ______/___[tissue face]_\______
        /          |  |  |  |           \
       |    LEFT   | forehead |  RIGHT   |
       |   temple  |          |  temple  |
                   ^
                   |
              Above left eyebrow,
              below hairline
```

---

## Step 8: Dark Test Verification

Before connecting to any electronics, verify your optical isolation.

### 8.1 Setup

- You will need the breadboard circuit from Part 2 for a full electrical test. However, you can do a preliminary visual check now.
- Take the probe into a completely dark room (or cover it with thick opaque material).
- Shine a bright flashlight directly at the back and sides of the probe.

### 8.2 Visual Check

- Look at the tissue-contact face. You should see zero light coming through anywhere except through the LED apertures and photodiode lens.
- If you see any light leaking through the foam body, patch it with additional tape or adhesive.

### 8.3 Electrical Dark Test (After Part 2 is Complete)

- Once the circuit is wired (Part 2), power on the LEDs one at a time with the probe pointing into a dark, sealed space.
- The ADC reading should be near zero (within noise floor).
- Any significant reading means light is leaking from the LED to the photodiode through the foam. **You must find and fix the leak before proceeding.**

---

## Common Mistakes

| Mistake | Consequence | Prevention |
|:--------|:-----------|:-----------|
| Swapped LED polarity | LED won't light or may be damaged | Label wires immediately after soldering |
| LED not flush with surface | Poor tissue coupling, weak signal | Check flush before glue sets |
| Photodiode lens facing wrong way | No signal detected | Convex lens = tissue side |
| Inadequate optical isolation | Signal overwhelmed by direct light | Perform dark test before proceeding |
| Photodiode wire too long | 60Hz noise corrupts signal | Keep shielded wire under 5cm |
| 30mm spacing incorrect | Wrong tissue depth sampled | Measure center-to-center carefully |

---

## Checkpoint

Before moving to Part 2, verify:

- [ ] Both LEDs are pigtail-wired with labeled polarity
- [ ] 730nm LED has washer baffle attached
- [ ] Foam block is cut to spec with photodiode hole and LED pockets at 30mm spacing
- [ ] Both LEDs mounted flush with tissue-contact surface
- [ ] BPW34 mounted with lens facing tissue side, shielded wire attached (< 5cm)
- [ ] Optical isolation is thorough - no visible light leaks
- [ ] Foam is attached to headband with comfort padding
- [ ] Wires are routed and managed along headband

**Proceed to [Part 2: Breadboard and Chip Wiring](Part2_Breadboard_and_Chip_Wiring.md)**
