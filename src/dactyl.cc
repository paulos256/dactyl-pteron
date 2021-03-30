#include <glm/glm.hpp>
#include <string>
#include <vector>

#include "key.h"
#include "key_data.h"
#include "scad.h"
#include "transform.h"

using namespace scad;

constexpr bool kWriteTestKeys = false;
// Add the caps into the stl for testing.
constexpr bool kAddCaps = false;

enum class Direction { UP, DOWN, LEFT, RIGHT };

void AddShapes(std::vector<Shape>* shapes, std::vector<Shape> to_add) {
  for (Shape s : to_add) {
    shapes->push_back(s);
  }
}

Shape ConnectMainKeys(KeyData& d);

int main() {
  printf("generating..\n");
  TransformList key_origin;
  key_origin.Translate(-20, -40, 3);

  // This is where all of the logic to position the keys is done. Everything below is cosmetic
  // trying to build the case.
  KeyData d(key_origin);

  if (kWriteTestKeys) {
    std::vector<Shape> test_shapes;
    std::vector<Key*> test_keys = {&d.key_e, &d.key_d, &d.key_r, &d.key_t, &d.key_d};
    for (Key* key : test_keys) {
      key->add_side_nub = false;
      key->extra_z = 4;
      test_shapes.push_back(key->GetSwitch());
      if (kAddCaps) {
        test_shapes.push_back(key->GetCap().Color("red"));
      }
    }
    UnionAll(test_shapes).WriteToFile("test_keys.scad");
    return 0;
  }

  // Set all of the widths here. This must be done before calling any of GetTopLeft etc.

  d.key_thumb1.extra_width_bottom = 2;
  d.key_thumb1.extra_width_left = 2;
  d.key_thumb2.extra_width_top = 2;
  d.key_thumb2.extra_width_bottom = 2;
  d.key_thumb3.extra_width_top = 2;
  d.key_thumb3.extra_width_bottom = 2;
  d.key_thumb4.extra_width_top = 2;
  d.key_thumb4.extra_width_right = 2;
  d.key_thumb4.extra_width_bottom = 2;

  // left wall
  for (Key* key : d.grid.column(0)) {
    if (key) {
      key->extra_width_left = 4;
    }
  }

  d.key_t.extra_width_right = 2;
  d.key_g.extra_width_right = 3;
  d.key_b.extra_width_right = 3;

  for (Key* key : d.grid.row(0)) {
    // top row
    if (key) {
      key->extra_width_top = 2;
    }
  }
  d.key_b.extra_width_bottom = 3;

  std::vector<Shape> shapes;

  //
  // Thumb plate
  //

  shapes.push_back(Union(ConnectHorizontal(d.key_thumb1, d.key_thumb2)
                        ,ConnectHorizontal(d.key_thumb2, d.key_thumb3)
                        ,ConnectHorizontal(d.key_thumb3, d.key_thumb4)
                        ,Tri(d.key_thumb1.GetTopLeft(),
                             d.key_thumb1.GetTopRight(),
                             d.key_thumb2.GetTopLeft())
                        ,Tri(d.key_thumb2.GetTopLeft(),
                             d.key_thumb2.GetTopRight(),
                             d.key_thumb3.GetTopLeft())
                         ));

  shapes.push_back(ConnectMainKeys(d));

  shapes.push_back(TriFan(d.key_thumb2.GetTopLeft(),
                          {
                              d.key_b.GetBottomRight(),
                              d.key_b.GetTopRight(),
                              d.key_g.GetBottomRight(),
                          })

  );

  // These transforms with TranslateFront are moving the connectors down in the z direction to
  // reduce the vertical jumps.
  TransformList slash_bottom_right = d.key_slash.GetBottomRight().TranslateFront(0, 0, -1);
  TransformList right_bottom_right = d.key_right_arrow.GetBottomRight().TranslateFront(0, 0, -3);

  shapes.push_back(TriFan(d.key_thumb1.GetTopLeft(),
                          {
                              right_bottom_right,
                              d.key_right_arrow.GetBottomLeft().TranslateFront(0, 0, -1),
                              d.key_left_arrow.GetBottomRight().TranslateFront(0, 0, -1),
                              d.key_left_arrow.GetBottomLeft().TranslateFront(0, 0, -1),
                              slash_bottom_right,
                              d.key_thumb1.GetBottomLeft(),
                          }));
  shapes.push_back(TriFan(d.key_thumb1.GetTopRight(),
                          {
                              d.key_thumb1.GetTopLeft(),
                              right_bottom_right,
                              d.key_b.GetBottomRight(),
                              d.key_thumb2.GetTopLeft(),
                          }));
  shapes.push_back(TriFan(d.key_thumb2.GetTopRight(),
                          {
                              d.key_thumb2.GetTopLeft(),
                              d.key_b.GetBottomRight(),
                              d.key_thumb3.GetTopLeft(),
                          }));
  shapes.push_back(TriFan(d.key_b.GetBottomLeft(),
                          {
                              d.key_b.GetBottomRight(),
                              right_bottom_right,
                              d.key_right_arrow.GetTopRight(),
                              d.key_v.GetBottomRight(),
                          }));
  shapes.push_back(TriFan(d.key_tilde.GetBottomRight(),
                          {
                              d.key_slash.GetBottomLeft(),
                              slash_bottom_right,
                          }));

  // Bottom right corner.
  shapes.push_back(TriFan(d.key_shift.GetBottomRight(),
                          {
                              d.key_z.GetBottomLeft(),
                              d.key_tilde.GetTopLeft(),
                              d.key_tilde.GetBottomLeft(),
                              d.key_shift.GetBottomLeft(),
                          }));

  //
  // Make the wall
  //
  {
    struct WallPoint {
      WallPoint(TransformList transforms,
                Direction out_direction,
                float extra_distance = 0,
                float extra_width = 0)
          : transforms(transforms),
            out_direction(out_direction),
            extra_distance(extra_distance),
            extra_width(extra_width) {
      }
      TransformList transforms;
      Direction out_direction;
      float extra_distance;
      float extra_width;
    };

    Direction up = Direction::UP;
    Direction down = Direction::DOWN;
    Direction left = Direction::LEFT;
    Direction right = Direction::RIGHT;

    std::vector<WallPoint> wall_points = {
        // Start top left and go clockwise
        {d.key_tab.GetTopLeft(), up},
        {d.key_tab.GetTopRight(), up, 0, .3},

        {d.key_q.GetTopLeft(), up, 0, .5},
        {d.key_q.GetTopRight().RotateFront(0, 0, 30), up, 0, 1},

        {d.key_w.GetTopLeft(), up, 0, .3},
        {d.key_w.GetTopRight(), up},

        {d.key_e.GetTopLeft(), up},
        {d.key_e.GetTopRight(), up},

        {d.key_r.GetTopLeft(), up},
        {d.key_r.GetTopRight(), up},
        {d.key_t.GetTopRight(), up},
        {d.key_t.GetTopRight(), right},
        {d.key_t.GetBottomRight(), right},

        {d.key_g.GetTopRight(), right},
        {d.key_g.GetBottomRight(), right, 1, .5},

        {d.key_b.GetTopRight(), right, 1, .5},
        {d.key_b.GetBottomRight(), right, 1, .5},

        // thumb plate
        {d.key_thumb3.GetTopLeft().RotateFront(0, 0, -25), up, 1, .5},
        {d.key_thumb3.GetTopRight(), up, 1, .5},
        {d.key_thumb4.GetTopLeft(), up, 1, .5},

        // round the corner
        {d.key_thumb4.GetTopRight(), up, 1, .5},
        {d.key_thumb4.GetTopRight(), right, 1, .5},
        {d.key_thumb4.GetBottomRight(), right, 1, .5},
        {d.key_thumb4.GetBottomRight(), down, 1, .5},
	// bottom edge
        {d.key_thumb4.GetBottomLeft(), down, 1, .5},
        {d.key_thumb3.GetBottomRight(), down, 1, .5},
        {d.key_thumb3.GetBottomLeft(), down, 1, .5},
        {d.key_thumb2.GetBottomRight(), down, 1, .5},
        {d.key_thumb2.GetBottomLeft(), down, 1, .5},
        {d.key_thumb1.GetBottomRight(), down, 1, .5},
        {d.key_thumb1.GetBottomLeft().RotateFront(0, 0, 15), down, 1, .5},
        {d.key_thumb1.GetBottomLeft(), left, 1, .5},
        // /thumb plate

        {slash_bottom_right.RotateFront(0, 0, -25), down},

        {d.key_tilde.GetBottomRight(), down},
        {d.key_tilde.GetBottomLeft(), down},

        {d.key_shift.GetBottomLeft(), down, 0, .75},
        {d.key_shift.GetBottomLeft(), left, 0, .5},
        {d.key_shift.GetTopLeft(), left, 0, .5},

        {d.key_caps.GetBottomLeft(), left},
        {d.key_caps.GetTopLeft(), left},

        {d.key_tab.GetBottomLeft(), left},
        {d.key_tab.GetTopLeft(), left},

        {d.key_tab.GetBottomLeft(), left},
        {d.key_tab.GetTopLeft(), left},
    };

    std::vector<std::vector<Shape>> wall_slices;
    for (WallPoint point : wall_points) {
      Shape s1 = point.transforms.Apply(GetPostConnector());

      TransformList t = point.transforms;
      glm::vec3 out_dir;
      float distance = 4.8 + point.extra_distance;
      switch (point.out_direction) {
        case Direction::UP:
          t.AppendFront(TransformList().Translate(0, distance, 0).RotateX(-20));
          break;
        case Direction::DOWN:
          t.AppendFront(TransformList().Translate(0, -1 * distance, 0).RotateX(20));
          break;
        case Direction::LEFT:
          t.AppendFront(TransformList().Translate(-1 * distance, 0, 0).RotateY(-20));
          break;
        case Direction::RIGHT:
          t.AppendFront(TransformList().Translate(distance, 0, 0).RotateY(20));
          break;
      }

      // Make sure the section extruded to the bottom is thick enough. With certain angles the
      // projection is very small if you just use the post connector from the transform. Compute
      // an explicit shape.
      const glm::vec3 post_offset(0, 0, -4);
      const glm::vec3 p = point.transforms.Apply(post_offset);
      const glm::vec3 p2 = t.Apply(post_offset);

      glm::vec3 out_v = p2 - p;
      out_v.z = 0;
      const glm::vec3 in_v = -1.f * glm::normalize(out_v);

      float width = 3.3 + point.extra_width;
      Shape s2 = Hull(Cube(.1).Translate(p2), Cube(.1).Translate(p2 + (width * in_v)));

      std::vector<Shape> slice;
      slice.push_back(Hull(s1, s2));
      slice.push_back(Hull(s2, s2.Projection().LinearExtrude(.1).TranslateZ(.05)));

      wall_slices.push_back(slice);
    }

    for (size_t i = 0; i < wall_slices.size(); ++i) {
      auto& slice = wall_slices[i];
      auto& next_slice = wall_slices[(i + 1) % wall_slices.size()];
      for (size_t j = 0; j < slice.size(); ++j) {
        shapes.push_back(Hull(slice[j], next_slice[j]));
        // Uncomment for testing. Much faster and easier to visualize.
        // shapes.push_back(slice[j]);
      }
    }
  }

  for (Key* key : d.all_keys()) {
    shapes.push_back(key->GetSwitch());
    if (kAddCaps) {
      shapes.push_back(key->GetCap().Color("red"));
    }
  }

  // Add all the screw inserts.
  std::vector<Shape> screw_holes;
  {
    double screw_height = 5;
    double screw_radius = 4.4 / 2.0;
    Shape screw_hole = Cylinder(screw_height + 2, screw_radius, 30);
    Shape screw_insert =
        Cylinder(screw_height, screw_radius + 1.65, 30).TranslateZ(screw_height / 2);

    glm::vec3 screw_left_bottom = d.key_shift.GetBottomLeft().Apply(kOrigin);
    screw_left_bottom.z = 0;
    screw_left_bottom.x += 3.2;

    glm::vec3 screw_left_top = d.key_tab.GetTopLeft().Apply(kOrigin);
    screw_left_top.z = 0;
    screw_left_top.x += 2.8;
    screw_left_top.y += -.5;

    glm::vec3 screw_right_top = d.key_t.GetTopRight().Apply(kOrigin);
    screw_right_top.z = 0;
    screw_right_top.x -= .8;
    screw_right_top.y += -.5;

    glm::vec3 screw_right_bottom = d.key_thumb1.GetBottomLeft().Apply(kOrigin);
    screw_right_bottom.z = 0;
    screw_right_bottom.y += 2.3;
    screw_right_bottom.x += 1.4;

    glm::vec3 screw_right_mid = d.key_thumb3.GetTopLeft().Apply(kOrigin);
    screw_right_mid.z = 0;
    screw_right_mid.y += -.9;

    shapes.push_back(Union(screw_insert.Translate(screw_left_top),
                           screw_insert.Translate(screw_right_top),
                           screw_insert.Translate(screw_right_mid),
                           screw_insert.Translate(screw_right_bottom),
                           screw_insert.Translate(screw_left_bottom)));
    screw_holes = {
        screw_hole.Translate(screw_left_top),
        screw_hole.Translate(screw_right_top),
        screw_hole.Translate(screw_right_mid),
        screw_hole.Translate(screw_right_bottom),
        screw_hole.Translate(screw_left_bottom),
    };
  }

  std::vector<Shape> negative_shapes;
  AddShapes(&negative_shapes, screw_holes);
//  Cut off the parts sticking up into the thumb plate.
//  negative_shapes.push_back(
//    d.key_thumb1.GetTopLeft().Apply(Cube(50, 50, 6).TranslateZ(3)).Color("red"));

  // Cut out holes for cords. Inserts can be printed to fit in.
  Shape connector_hole = Cube(10, 20, 10).TranslateZ(12 / 2);
  glm::vec3 connector_location1 = d.key_r.GetTopLeft().Apply(kOrigin);
  connector_location1.z = 6;
  connector_location1.x += 9.75;
  glm::vec3 connector_location2 = d.key_t.GetTopLeft().Apply(kOrigin);
  connector_location2.z = 6;
  connector_location2.x += 10.5;
  negative_shapes.push_back(connector_hole.Translate(connector_location1));
  negative_shapes.push_back(connector_hole.Translate(connector_location2));

  Shape result = UnionAll(shapes);
  // Subtracting is expensive to preview and is best to disable while testing.
  result = result.Subtract(UnionAll(negative_shapes));
  result.WriteToFile("v1_left.scad");
  result.MirrorX().WriteToFile("v1_right.scad");

  {
    double depth = 13;
    double width = 10;
    double top_width = width;
    double mid_width = 6;
    double mid_height = 5.75;
    // Height of the bottom plate on the trrs jack.
    double bottom_plate_height = 2;

    Shape bottom_plate =
        Cube(width, 2, depth).TranslateY(-1 - bottom_plate_height).TranslateZ(depth / 2);
    Shape bottom_face = Cube(width, 5, 2).Translate(0, -(5 / 2 + 3), 1);
    Shape top_face = Cube(top_width, 5, 2).Translate(0, 5 / 2 + mid_height + 1, 1);

    Shape top_plate = Cube(top_width, 2, depth).TranslateY(1 + mid_height).TranslateZ(depth / 2);
    double back_height = mid_height + 6;
    Shape back_plate = Cube(top_width, back_height, 2).Translate(0, back_height / 2 - 4, 1 + depth);

    Union(top_face, bottom_plate, bottom_face, top_plate, back_plate).WriteToFile("trrs.scad");
  }

  {
    // trrs front plate
    double inner_radius = 9.8 / 2;
    double width = 3;
    double depth = 1;
    double fn = 20;

    Circle(inner_radius + width, fn)
        .Subtract(Circle(inner_radius, fn))
        .LinearExtrude(depth)
        .WriteToFile("trrs_front.scad");
    Square(13).LinearExtrude(depth).WriteToFile("cover.scad");
  }

  {
    // usbc adapter
    double width = 11.8;
    double height = 7.4;

    double thickness = 4;
    double depth = 7;

    Square(11.8 + thickness * 2, 7.4 + thickness * 2)
        .Subtract(Square(11.8, 7.4))
        .LinearExtrude(depth)
        .WriteToFile("usbc.scad");
  }

  // Bottom plate
  {
    std::vector<Shape> bottom_plate_shapes = {result};
    for (Key* key : d.all_keys()) {
      bottom_plate_shapes.push_back(Hull(key->GetSwitch()));
    }

    Shape bottom_plate = UnionAll(bottom_plate_shapes)
                             .Projection()
                             .LinearExtrude(1.5)
                             .Subtract(UnionAll(screw_holes));
    bottom_plate.WriteToFile("v1_bottom_left.scad");
    bottom_plate.MirrorX().WriteToFile("v1_bottom_right.scad");
  }

  return 0;
}

Shape ConnectMainKeys(KeyData& d) {
  std::vector<Shape> shapes;
  for (int r = 0; r < d.grid.num_rows(); ++r) {
    for (int c = 0; c < d.grid.num_columns(); ++c) {
      Key* key = d.grid.get_key(r, c);
      if (!key) {
        // No key at this location.
        continue;
      }
      Key* left = d.grid.get_key(r, c - 1);
      Key* top_left = d.grid.get_key(r - 1, c - 1);
      Key* top = d.grid.get_key(r - 1, c);

      if (left) {
        shapes.push_back(ConnectHorizontal(*left, *key));
      }
      if (top) {
        shapes.push_back(ConnectVertical(*top, *key));
        if (left && top_left) {
          shapes.push_back(ConnectDiagonal(*top_left, *top, *key, *left));
        }
      }
    }
  }
  return UnionAll(shapes);
}
