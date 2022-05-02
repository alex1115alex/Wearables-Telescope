import "dart:math";

String getFakeDrinkName() {
  Random random = new Random();
  int randomNumber = random.nextInt(2);
  if (randomNumber == 0) {
    return generateFakeDrinkNameFromLists();
  } else {
    Random randA = new Random();
    return predefinedDict[randA.nextInt(predefinedDict.length)];
  }
}

String generateFakeDrinkNameFromLists() {
  Random randA = new Random();
  Random randB = new Random();
  Random randC = new Random();
  String name = "";
  name += colA[randA.nextInt(colA.length)] + " ";
  name += colB[randB.nextInt(colB.length)] + " ";
  name += colC[randC.nextInt(colC.length)];
  return name;
}

const predefinedDict = [
  "Alaskan Pipeline",
  "Dirty Sanchez",
  "Alabama hot pocket",
  "Rusty Trombone",
  "Irish Waffle Stomp",
  "Cleveland Steamer",
  "Spaghetti and meatballs",
  "Wet dog in a bathtub",
  "Dirty blumpkin",
  "Golden shower",
  ""
];

// Define word lists
const colA = [
  'Kinky',
  'Kilted',
  'Sexed',
  'Iced Out',
  'Hot and Buttered',
  'Hot',
  'Oiled up',
  'Freaky',
  'Foxy',
  'Flexing',
  'Fizzy',
  'Exploding',
  'Elegant',
  'Dirty',
  'Devious',
  'Crunk',
  'Creamy',
  'Cranberry',
  'Cheeky',
  "Blackbeard's",
  'Minty',
  "Bangin'",
  'Banana',
  'Bahama',
  'Officer Friendly\'s',
  'Naughty',
  'Eggplant',
  'Assaulting',
  'Ejaculating',
  'Southern',
  'Spicy',
  'Stay Woke',
  'Stinging',
  'Supreme',
  'Tipsy',
  'Turnt',
  'Wet'
];
const colB = [
  'Hipster',
  'Hippo',
  'Haze',
  'Hawaiian',
  'Gnome',
  'Gangster',
  'Frenchman',
  'Homosexual',
  'Irish',
  'Elephant',
  'Ecstasy',
  'Heroin',
  'Dickens',
  'Diablo',
  'Irishman',
  'Dazzle',
  'Canadian',
  'Butler',
  'Boss',
  'Boogie',
  'Blues',
  'Bite',
  'Beaver',
  'Penis',
  'Gay',
  'Jolly Roger',
  'Autism',
  'Kitten',
  'Baller',
  'Hyena',
  'Love Boat',
  'Manchild',
  'Maverick',
  'Minion',
  'Terminator',
  'Mobster',
  'Cosby',
  'Russian',
  'Scorpion',
  'Taxman',
  'Tickler',
  'Tiger',
  'T-Rex',
  'Trickster',
  'Shit Sock'
];
const colC = [
  'Blitz',
  'Breeze',
  'Buzz',
  'Chiller',
  'Collins',
  'Cosmo',
  'Crush',
  'Cup',
  'Cure',
  'Fetish toy',
  'Gimlet',
  'Goblet',
  'Grog',
  'Julep',
  'Kiss',
  'Last Word',
  'Margarita',
  'Martini ',
  'Side Car',
  'Sling',
  'Slushie',
  'Sour',
  'Leather whip',
  'Special',
  'Vodka Tonic',
  'Child beater',
  'Vomit comet',
  'Piss pool',
  'Shit Sock'
];
