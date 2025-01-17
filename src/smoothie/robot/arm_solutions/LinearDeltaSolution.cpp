#include "LinearDeltaSolution.h"

#include "smoothie/smoothie/ConfigReader.h"
#include "libs/Vector3.h"
#include "smoothie/smoothie/AxisDefns.h"

#include <math.h>
#include <stdio.h>

#define arm_length_key         "arm_length"
#define arm_radius_key         "arm_radius"

#define tower1_offset_key      "delta_tower1_offset"
#define tower2_offset_key      "delta_tower2_offset"
#define tower3_offset_key      "delta_tower3_offset"
#define tower1_angle_key       "delta_tower1_angle"
#define tower2_angle_key       "delta_tower2_angle"
#define tower3_angle_key       "delta_tower3_angle"

#define SQ(x) ((x)*(x))
#define ROUND(x, y) (roundf((x) * (float)(1e ## y)) / (float)(1e ## y))
#define PIOVER180   0.01745329251994329576923690768489F

LinearDeltaSolution::LinearDeltaSolution(ConfigReader& cr)
{
    ConfigReader::section_map_t m;
    if(cr.get_section("linear delta", m)) {
        // arm_length is the length of the arm from hinge to hinge
        arm_length = cr.get_float(m, arm_length_key, 250.0f);
        // arm_radius is the horizontal distance from hinge to hinge when the effector is centered
        arm_radius = cr.get_float(m, arm_radius_key, 124.0f);

        tower1_angle = cr.get_float(m, tower1_angle_key, 0.0f);
        tower2_angle = cr.get_float(m, tower2_angle_key, 0.0f);
        tower3_angle = cr.get_float(m, tower3_angle_key, 0.0f);
        tower1_offset = cr.get_float(m, tower1_offset_key, 0.0f);
        tower2_offset = cr.get_float(m, tower2_offset_key, 0.0f);
        tower3_offset = cr.get_float(m, tower3_offset_key, 0.0f);
    }else{
        printf("WARNING:config-LinearDeltaSolution: No linear delta section found, defaults used\n");
    }

    init();
}

void LinearDeltaSolution::init()
{
    arm_length_squared = SQ(arm_length);

    // Effective X/Y positions of the three vertical towers.
    float delta_radius = arm_radius;

    delta_tower1_x = (delta_radius + tower1_offset) * cosf((210.0F + tower1_angle) * PIOVER180); // front left tower
    delta_tower1_y = (delta_radius + tower1_offset) * sinf((210.0F + tower1_angle) * PIOVER180);
    delta_tower2_x = (delta_radius + tower2_offset) * cosf((330.0F + tower2_angle) * PIOVER180); // front right tower
    delta_tower2_y = (delta_radius + tower2_offset) * sinf((330.0F + tower2_angle) * PIOVER180);
    delta_tower3_x = (delta_radius + tower3_offset) * cosf((90.0F  + tower3_angle) * PIOVER180); // back middle tower
    delta_tower3_y = (delta_radius + tower3_offset) * sinf((90.0F  + tower3_angle) * PIOVER180);
}

void LinearDeltaSolution::cartesian_to_actuator(const float cartesian_mm[], ActuatorCoordinates &actuator_mm ) const
{

    actuator_mm[ALPHA_STEPPER] = sqrtf(this->arm_length_squared
                                       - SQ(delta_tower1_x - cartesian_mm[X_AXIS])
                                       - SQ(delta_tower1_y - cartesian_mm[Y_AXIS])
                                      ) + cartesian_mm[Z_AXIS];
    actuator_mm[BETA_STEPPER ] = sqrtf(this->arm_length_squared
                                       - SQ(delta_tower2_x - cartesian_mm[X_AXIS])
                                       - SQ(delta_tower2_y - cartesian_mm[Y_AXIS])
                                      ) + cartesian_mm[Z_AXIS];
    actuator_mm[GAMMA_STEPPER] = sqrtf(this->arm_length_squared
                                       - SQ(delta_tower3_x - cartesian_mm[X_AXIS])
                                       - SQ(delta_tower3_y - cartesian_mm[Y_AXIS])
                                      ) + cartesian_mm[Z_AXIS];
}

void LinearDeltaSolution::actuator_to_cartesian(const ActuatorCoordinates &actuator_mm, float cartesian_mm[] ) const
{
    // from http://en.wikipedia.org/wiki/Circumscribed_circle#Barycentric_coordinates_from_cross-_and_dot-products
    // based on https://github.com/ambrop72/aprinter/blob/2de69a/aprinter/printer/DeltaTransform.h#L81
    Vector3 tower1( delta_tower1_x, delta_tower1_y, actuator_mm[0] );
    Vector3 tower2( delta_tower2_x, delta_tower2_y, actuator_mm[1] );
    Vector3 tower3( delta_tower3_x, delta_tower3_y, actuator_mm[2] );

    Vector3 s12 = tower1.sub(tower2);
    Vector3 s23 = tower2.sub(tower3);
    Vector3 s13 = tower1.sub(tower3);

    Vector3 normal = s12.cross(s23);

    float magsq_s12 = s12.magsq();
    float magsq_s23 = s23.magsq();
    float magsq_s13 = s13.magsq();

    float inv_nmag_sq = 1.0F / normal.magsq();
    float q = 0.5F * inv_nmag_sq;

    float a = q * magsq_s23 * s12.dot(s13);
    float b = q * magsq_s13 * s12.dot(s23) * -1.0F; // negate because we use s12 instead of s21
    float c = q * magsq_s12 * s13.dot(s23);

    Vector3 circumcenter( delta_tower1_x * a + delta_tower2_x * b + delta_tower3_x * c,
                          delta_tower1_y * a + delta_tower2_y * b + delta_tower3_y * c,
                          actuator_mm[0] * a + actuator_mm[1] * b + actuator_mm[2] * c );

    float r_sq = 0.5F * q * magsq_s12 * magsq_s23 * magsq_s13;
    float dist = sqrtf(inv_nmag_sq * (arm_length_squared - r_sq));

    Vector3 cartesian = circumcenter.sub(normal.mul(dist));

    cartesian_mm[0] = ROUND(cartesian[0], 4);
    cartesian_mm[1] = ROUND(cartesian[1], 4);
    cartesian_mm[2] = ROUND(cartesian[2], 4);
}

bool LinearDeltaSolution::set_optional(const arm_options_t& options)
{

    for(auto &i : options) {
        switch(i.first) {
            case 'L': arm_length = i.second; break;
            case 'R': arm_radius = i.second; break;
            case 'A': tower1_offset = i.second; break;
            case 'B': tower2_offset = i.second; break;
            case 'C': tower3_offset = i.second; break;
            case 'D': tower1_angle = i.second; break;
            case 'E': tower2_angle = i.second; break;
            case 'H': tower3_angle = i.second; break;
        }
    }
    init();
    return true;
}

bool LinearDeltaSolution::get_optional(arm_options_t& options, bool force_all) const
{
    options['L'] = this->arm_length;
    options['R'] = this->arm_radius;

    // don't report these if none of them are set
    if(force_all || (this->tower1_offset != 0.0F || this->tower2_offset != 0.0F || this->tower3_offset != 0.0F ||
                     this->tower1_angle != 0.0F  || this->tower2_angle != 0.0F  || this->tower3_angle != 0.0F) ) {

        options['A'] = this->tower1_offset;
        options['B'] = this->tower2_offset;
        options['C'] = this->tower3_offset;
        options['D'] = this->tower1_angle;
        options['E'] = this->tower2_angle;
        options['H'] = this->tower3_angle;
    }

    return true;
};
