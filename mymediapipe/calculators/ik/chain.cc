#include "chain.h"

Chain::Chain(std::vector<glm::vec3> joints, Target* t) {
    std::vector<float> lengths;
    std::vector<glm::quat> directions;
    CalculateLinks(joints, &lengths, &directions);
    
    for(int i = 0; i < lengths.size(); ++i) {
        segments.push_back(Segment(joints[i], joints[i+1], lengths[i], directions[i]));
        total_length += lengths[i];
    }
    
    target = t;
    size = joints.size();
    this->joints = joints;
}

void Chain::Solve() {
    float current_distance = glm::length(target->position - origin);
  
    // Target is out of reach; fully extend the arm
    if (current_distance > total_length) {
        for(int i = 0; i < joints.size() - 1; ++i) {
            float r = glm::length(target->position - joints[i]);
            float l = segments[i].magnitude / r;
            joints[i + 1] = (1 - l) * joints[i] + l * target->position;
        }
        
        std::vector<float> lengths;
        std::vector<glm::quat> directions;
        CalculateLinks(joints, &lengths, &directions);
    } else { // Target is in reach, use FABRIK to solve both back and forwards
        int count = 0;
        float difference = glm::length(joints[joints.size() - 1] - target->position);
        while (difference > tolerance || count < 1) {
            Backward();
            Forward();
            difference = glm::length(joints[joints.size() - 1] - target->position);
            count++;
            
            if (count > 20) break;
        }
    }
    
    SetSegments();
}

void Chain::SetSegments() {
    std::vector<float> lengths;
    std::vector<glm::quat> directions;
    CalculateLinks(joints, &lengths, &directions);
    
    for (int i = 0; i < lengths.size(); ++i) {
        segments[i].Set(joints[i], joints[i+1], lengths[i], directions[i]);
    }
}

void Chain::Backward() {
    auto end = joints.end() - 1;
    *end = target->position;
    for (int i = int(joints.size() - 2); i >= 0; --i) {
        float r = glm::length(joints[i+1] - joints[i]);
        float l = segments[i].magnitude / r;
        joints[i] = (1 - l) * joints[i+1] + l * joints[i];
    }
}

void Chain::Forward() {
    auto beg = joints.begin();
    *beg = origin;
    for (int i = 0; i < joints.size() - 1; ++i) {
        float r = glm::length(joints[i+1] - joints[i]);
        float l = segments[i].magnitude / r;
        
        glm::vec3 new_point = (1 - l) * joints[i] + l * joints[i+1];
        if (i > 0 && this->please_constrain) 
            new_point = Constrain(new_point, segments[i].magnitude, &(segments[i - 1]));
        joints[i + 1] = new_point;
    }
}

glm::vec3 Chain::Constrain(glm::vec3 point, float true_length, Segment *seg) {
  
    glm::vec3 retval = point;
    glm::vec3 relative_point = point - seg->end_position;
    bool debug = true;
    
    // Get projection of the new calculated point onto the direction the previous segment is pointing in
    glm::vec3 line_dir = glm::normalize(seg->GetConstraintConeAxis());
    float     scalar = glm::dot(point - seg->end_position, line_dir);
    glm::vec3 projected_point = scalar * line_dir + seg->end_position;
    
    
    if (debug) {
        float angle = glm::acos(scalar/ (glm::length(point - seg->end_position) * glm::length(line_dir)));
        std::cout << "angle: " << glm::degrees(angle) << std::endl;
        std::cout << "scalar: " << scalar << std::endl;
        std::cout << "projected point: " << projected_point.x << " " << projected_point.y << " " << projected_point.z << " " << std::endl;
    }
    
    glm::vec3 adjusted_distance = point - projected_point;
    if (scalar < 0) {
        glm::vec3 relative_projected_point = projected_point - seg->end_position;
        relative_projected_point = - relative_projected_point;
        projected_point = relative_projected_point + seg->end_position;
    }

    // Get the 2D axes we will be using for this problem
    glm::mat4 face_normals = seg->GetFaceNormals();
    glm::vec3 up =    glm::normalize(glm::vec3(face_normals[0]));
    glm::vec3 down =  glm::normalize(glm::vec3(face_normals[1]));
    glm::vec3 left =  glm::normalize(glm::vec3(face_normals[2]));
    glm::vec3 right = glm::normalize(glm::vec3(face_normals[3]));
    
    glm::vec3 x_axis = glm::length(left - relative_point) < glm::length(right - relative_point) ? left : right;
    glm::vec3 y_axis = glm::length(up - relative_point) < glm::length(down - relative_point) ? up : down;
    
    float x_aspect = glm::dot(adjusted_distance, x_axis);
    float y_aspect = glm::dot(adjusted_distance, y_axis);
    
    if (debug) {
        std::cout << glm::radians(seg->constraint_cone[0]) << std::endl;
        std::cout << glm::radians(seg->constraint_cone[0]) << std::endl;
        std::cout << glm::tan(glm::radians(seg->constraint_cone[0])) << std::endl;
        std::cout << "x axis: " << x_axis.x << " " << x_axis.y << " " << x_axis.z << std::endl;
    }
  
    // Calculate the cone cross section
    float proj_length = glm::length(projected_point - seg->end_position);
    float up_cross = proj_length * glm::tan(glm::radians(seg->constraint_cone[0]));
    float down_cross = -(proj_length * glm::tan(glm::radians(seg->constraint_cone[1])));
    float left_cross = -(proj_length * glm::tan(glm::radians(seg->constraint_cone[2])));
    float right_cross = proj_length * glm::tan(glm::radians(seg->constraint_cone[3]));
    
    // See which quadrant we should use
    float x_bound = x_aspect >= 0 ? right_cross : left_cross;
    float y_bound = y_aspect >= 0 ? up_cross : down_cross;
    
    // Calculate where our point lies on the ellipse (finally)
    float ellipse_point = glm::pow(x_aspect, 2) / glm::pow(x_bound, 2) + glm::pow(y_aspect, 2) / glm::pow(y_bound, 2);
    
    //if(debug) cout << "ellipse point: " << ellipse_point << endl;
    std::cout << "y_aspect " << y_aspect << "  x_aspect " << x_bound << std::endl;
    
    // If the point we calculated is outside of this ellipse, then we must constrain the joint
    if (ellipse_point > 1 || scalar < 0) {
        std::cout << "Not in bounds!" << std::endl;
        float a = glm::atan(y_aspect, x_aspect);
        float x = x_bound * glm::cos(a);
        float y = y_bound * glm::sin(a);
        
        retval = glm::normalize((projected_point - seg->end_position) + (x_axis * x) + (y_axis * y)) * glm::length(relative_point) + seg->end_position;
    } else {
        std::cout << "In bounds!" << std::endl;
    }

    return retval;
}

void Chain::CalculateLinks(std::vector<glm::vec3> joints, std::vector<float> *lengths, std::vector<glm::quat> *directions) {
  
    origin = *joints.begin();
    end = *(joints.end() - 1);
    
    for (auto it = joints.begin(); it != joints.end() - 1; ++it) {
        glm::vec3 current = *it;
        glm::vec3 next = *(it + 1);
        glm::vec3 link_vector = next - current;
        
        // Get magnitude of link
        lengths->push_back(glm::length(link_vector));
        
        // Get rotation of link
        glm::vec3 cross = glm::cross(ref_rot_vector, link_vector);
        glm::quat q;
        q.x = cross.x;
        q.y = cross.y;
        q.z = cross.z;
        q.w = sqrt(pow(glm::length(link_vector), 2) * pow(glm::length(ref_rot_vector), 2)) + glm::dot(link_vector, ref_rot_vector);
        directions->push_back(glm::normalize(q));
    }
}

void Chain::Render(glm::mat4 view, glm::mat4 proj) {
    for (auto it = segments.begin(); it != segments.end(); ++it) {
        it->Render(view, proj);
    }
}