#pragma once

class Color {
 public:
  Color();
  Color(double red, double green, double blue);
  double GetRed() const;
  double GetGreen() const;
  double GetBlue() const;
  void SetRed(double red);
  void SetGreen(double green);
  void SetBlue(double blue);
  static double Clamp(double value);

 private:
  double red_;
  double green_;
  double blue_;
};
