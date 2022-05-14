/*******************************************************************
 *                                                                 *
 * Addressable LED Pixel Pattern Framework                         *
 *                                                                 *
 * Controller Class                                                *
 *                                                                 *
 * by Ross Butler   Dec. 2015                                      *
 *                                                                 *
 *******************************************************************/

#include "FastLED.h"
#include "PixelPatternController.h"
#include "pixelPatternFrameworkTypes.h"
#include "PatternSequence.h"
#include "PixelPattern.h"
#include "pixelPatternFactory.h"
#include "PixelSet.h"


using namespace pixelPattern;


void PixelPatternController::enableStatusLed(int8_t pin)
{
    useStatusLed = true;
    statusLedPin = pin;
}


uint8_t PixelPatternController::addPatternSequence(PatternSequence* patternSequence, PixelSet* pixelSet)
{
    if (numPatternSequences >= maxPatternSequences || 0 == patternSequence || 0 == pixelSet) {
        return UINT8_MAX;
    }

    PatternState* ps = new PatternState;

    ps->patternSequence = patternSequence;
    ps->pixelSet = pixelSet;
    ps->pixPat = 0;
    ps->loopedWithoutPatternUpdate = false;
    ps->timingSatisfied = false;
    ps->updateLeds = false;

    uint8_t patternSequenceIdx = numPatternSequences++;

    patternStates[patternSequenceIdx] = ps;

    return patternSequenceIdx;
}


bool PixelPatternController::getUpdateLeds(uint8_t patternSequenceIdx)
{
    return patternSequenceIdx < numPatternSequences ? patternStates[patternSequenceIdx]->updateLeds : false;
}


void PixelPatternController::init()
{
    // Initialize the pixel arrays so that any skipped pixels will remain off.
    for (uint8_t psidx = 0; psidx < numPatternSequences; ++psidx) {
        PatternState* ps = patternStates[psidx];
        fill_solid(ps->pixelSet->allPixels, ps->pixelSet->numPhysicalPixels, CRGB::Black);
    }

    initDone = true;
}


void PixelPatternController::displayRelativeValue(PixelSet* pixelSet, uint8_t value, CRGB rgbColor)
{
  // Displays a bar of illuminated LEDs corresponding to value (0 to 255).

  uint32_t numPixelsOn = value * pixelSet->numSymmetricalPixels / UINT8_MAX;

  fill_solid(pixelSet->pixels, numPixelsOn, rgbColor);

  // TODO:  use a FastLED scaling function
  for (uint16_t i = 0; i < numPixelsOn; pixelSet->pixels[i++].nscale8_video(pixelSet->backgroundIntensityScaleFactor));

  FastLED.show();
}


void PixelPatternController::updatePattern(PatternState& patternState)
{
    patternState.updateLeds = false;

    if (patternState.patternSequence->patternChangeRequested()) {
//Serial.println("need pattern change");
        patternState.patternSequence->changePattern();

        // Turn off all the pixels, including skipped and non-pattern pixels.
        fill_solid(patternState.pixelSet->allPixels, patternState.pixelSet->numPhysicalPixels, CRGB::Black);

        uint8_t patternId;
        void* patternConfig;
        patternState.patternSequence->readCurrentPatternDefinitionFromFlash(&patternId, nullptr, &patternConfig);

//=-=-=-=-=-=
//Serial.print("patternDefs=");
//Serial.print((uint32_t) patternState.patternSequence->patternDefs);
//Serial.print("  patternNum=");
//Serial.print(patternState.patternSequence->patternNum);
//Serial.print("  patternId=");
//Serial.print(patternId);
//Serial.print("  patternConfig=");
//Serial.println((uint32_t) patternConfig);

        if (0 != patternState.pixPat) {
            delete patternState.pixPat;
        }
        patternState.pixPat = pixelPatternFactory(patternId);
//Serial.println("created pixPat object");

        if (patternState.pixPat->init(true, patternConfig, patternState.pixelSet)) {
//Serial.println("pixPat->init successful");
            // Blip the can't-keep-up light.
            patternState.timingSatisfied = false;
        }
        else {
//Serial.println("pixPat->init failed");
            // The pattern can't run.  We'll delete the object and set pixPat
            // to 0 so that we don't try to update the pattern.  Hopefully,
            // the next pattern change will give us one we can run.
            delete patternState.pixPat;
            patternState.pixPat = 0;
            patternState.timingSatisfied = true;
        }

        patternState.updateLeds = false;
    }
  
    else {
//Serial.println("no pattern change");

        if (patternState.pixPat != 0 && millis() >= patternState.pixPat->nextUpdateMs) {

            // If loop has been called at least once without having to call
            // pixPat->update() then we are probably keeping up with the update
            // frequency that the pattern wants.  However, if we are calling
            // pixPat->update() every time loop is called, we probably can't satisfy
            // the pattern's timing.
            patternState.timingSatisfied = patternState.loopedWithoutPatternUpdate;
            patternState.loopedWithoutPatternUpdate = false;
      
//Serial.println("calling pixPat-update");
            patternState.updateLeds = patternState.pixPat->update();
//Serial.println("back from pixPat-update");
        }
        else {
            patternState.loopedWithoutPatternUpdate = true;
        }

    }  

}


bool PixelPatternController::update()
{
    bool writeToLeds = false;
    bool allTimingSatisfied = true;
    for (uint8_t psidx = 0; psidx < numPatternSequences; ++psidx) {
        PatternState* ps = patternStates[psidx];
        updatePattern(*ps);
        writeToLeds |= ps->updateLeds;
        allTimingSatisfied &= ps->timingSatisfied;
    }

    if (writeToLeds) {
        FastLED.show();
    }

    if (useStatusLed) {
        digitalWrite(statusLedPin, !allTimingSatisfied);
    }

    return writeToLeds;
}

