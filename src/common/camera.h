#ifndef CAMERA_H
#define CAMERA_H
//==============================================================================================
// Originally written in 2016 by Peter Shirley <ptrshrl@gmail.com>
//
// To the extent possible under law, the author(s) have dedicated all copyright and related and
// neighboring rights to this software to the public domain worldwide. This software is
// distributed without any warranty.
//
// You should have received a copy (see file COPYING.txt) of the CC0 Public Domain Dedication
// along with this software. If not, see <http://creativecommons.org/publicdomain/zero/1.0/>.
//==============================================================================================

#include "rtweekend.h"


class camera 
{
    public:
        camera() : camera(point3(0,0,-1), point3(0,0,0), vec3(0,1,0), 40, 1, 0, 10) {}

        camera(
            point3 lookfrom,
            point3 lookat,
            vec3   vup,
            double vfov, // vertical field-of-view in degrees
            double aspect_ratio,
            double aperture,
            double focus_dist,
            double _time0 = 0,
            double _time1 = 0
        ) 
        {
            // #_1_04_02 : Sending Rays Into the Scene
            // focal length : projection point와 image plane과의 거리
            // projection point : camera의 origin
            // focal_lengh 는 1로 놓고 계산
            
            auto theta = degrees_to_radians(vfov);
            auto h = tan(theta/2);
            auto viewport_height = 2.0 * h;
            auto viewport_width = aspect_ratio * viewport_height;

            // UVN : UVW
            w = unit_vector(lookfrom - lookat);
            u = unit_vector(cross(vup, w));
            v = cross(w, u);

            // virtual film plane at the focus_dist
            // virtual film plane을 image plane로 사용
            origin = lookfrom;
            horizontal = focus_dist * viewport_width * u;
            vertical = focus_dist * viewport_height * v;
            lower_left_corner = origin - horizontal/2 - vertical/2 - focus_dist*w;

            lens_radius = aperture / 2;
            time0 = _time0;
            time1 = _time1;
        }

        ray get_ray(double s, double t) const 
        {
        #define DOF 1   // #_1_12 : Defocus Blur
        #if DOF
            vec3 rd = lens_radius * random_in_unit_disk();
            vec3 offset = u * rd.x() + v * rd.y();
        #else
            vec3 offset = vec3(0, 0, 0);
        #endif
            return ray(
                origin 
                + offset,   // lens aperture
                lower_left_corner + s*horizontal + t*vertical - origin 
                - offset,   // orgin이 올라간 만큼 dir을 내려야, 원래 target에 도착
                random_double(time0, time1)
            );
        }

    private:
        point3 origin;
        point3 lower_left_corner;
        vec3 horizontal;
        vec3 vertical;
        vec3 u, v, w;
        double lens_radius;
        double time0, time1;  // shutter open/close times
};

#endif
