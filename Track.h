/**
 * \file Track.h
 * \author Matt Hammerly
 * \brief Contains the definition of the Track class
 */

#ifndef TRACK_H
#define TRACK_H

/**
 * \brief This class represents a track in the library
 *
 * Dealing with database records is kind of ugly so we'll
 * use these when we can.
 */
class CTrack
{
public:

    /** \brief Default constructor (disabled) */
    CTrack() = delete;

    /** \brief Copy constructor (disabled)
     * \param track Track to construct this based on */
    CTrack(const CTrack &track) = delete;

    /** \brief Assignment operator (disabled)
     * \param track Track whose attributes will override those of the current track */
    CTrack& operator=(const CTrack &track) = delete;

    /** \brief Destructor */
    ~CTrack() {}
};

#endif
