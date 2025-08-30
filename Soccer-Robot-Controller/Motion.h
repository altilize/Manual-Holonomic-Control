int mixedCubicMapping(int rawInput, float maxSpeed, float cubicWeight = 1) {
  if (rawInput > 512) rawInput = 512;
  if (rawInput < -511) rawInput = -511;

  if (rawInput > 490) rawInput = 512;
  if (rawInput < -490) rawInput = -512;

  float norm = rawInput / 512.0;

  float mix = (1.0 - cubicWeight) * norm + cubicWeight * (norm * norm * norm);

  int speed = (int)(mix * maxSpeed);

  return speed;
}

int exponentialMapping(int rawInput, float maxSpeed, float expWeight = 1) {
  if (rawInput > 512) rawInput = 512;
  if (rawInput < -511) rawInput = -511;

  if (rawInput > 490) rawInput = 512;
  if (rawInput < -490) rawInput = -512;

  // Normalisasi input
  float norm = rawInput / 512.0;

  // Eksponensial Mapping dengan pembobotan
  float expMapped = (1.0 - expWeight) * norm + expWeight * (pow(norm, 2));

  int speed = (int)(expMapped * maxSpeed);

  if (rawInput < 0) {
    return speed * -1;
  } else {
    return speed;
  }
}
