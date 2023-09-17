
import numpy as np
import sys

def generate_uv_sphere(radius=1.0, segments=32, rings=16):
    vertices = []
    indices = []
    
    for ring in range(rings + 1):
        theta = np.pi * ring / rings
        sin_theta = np.sin(theta)
        cos_theta = np.cos(theta)
        
        for segment in range(segments + 1):
            phi = 2 * np.pi * segment / segments
            sin_phi = np.sin(phi)
            cos_phi = np.cos(phi)
            
            x = cos_phi * sin_theta
            y = cos_theta
            z = sin_phi * sin_theta
            
            vertices.extend([radius * x, radius * y, radius * z])
            
            if ring != rings:
                index1 = ring * (segments + 1) + segment
                index2 = index1 + segments + 1
                indices.extend([index1, index2])
                
    return vertices, indices

if __name__ == "__main__":
    if len(sys.argv) != 2:
        print("Usage: python uv_sphere_generator.py <resolution>")
        sys.exit(1)

    resolution = int(sys.argv[1])
    vertices, indices = generate_uv_sphere(segments=resolution, rings=resolution)

    with open("sphere_vertices.dat", "w") as f:
        for v in vertices:
            f.write(str(v) + "\n")

    with open("sphere_indices.dat", "w") as f:
        for idx in indices:
            f.write(str(idx) + "\n")
