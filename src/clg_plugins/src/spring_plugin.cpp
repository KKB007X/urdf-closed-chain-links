#include <gz/sim/System.hh>
#include <gz/sim/Model.hh>
#include <gz/plugin/Register.hh>
#include <gz/sim/components/Name.hh>
#include <gz/sim/components/Pose.hh>
#include <gz/sim/components/ExternalWorldWrenchCmd.hh>
#include <gz/math/Vector3.hh>
#include <gz/sim/Link.hh>

namespace clg_plugins
{

class SpringPlugin:
    public gz::sim::System,
    public gz::sim::ISystemConfigure,
    public gz::sim::ISystemPreUpdate
{
public:

    void Configure(
        const gz::sim::Entity &entity,
        const std::shared_ptr<const sdf::Element> &,
        gz::sim::EntityComponentManager &ecm,
        gz::sim::EventManager &) override
    {
        std::cout << "SpringPlugin loaded!" << std::endl;

        std::cout << "Robot entity = "
                << entity
                << std::endl;
    }

    void PreUpdate(
        const gz::sim::UpdateInfo &,
        gz::sim::EntityComponentManager &ecm) override
    {
        counter++;

        if (!printedLinks)
        {
            printedLinks = true;

            ecm.Each<gz::sim::components::Name>(
            [&](const gz::sim::Entity &entity,
                const gz::sim::components::Name *name)
            {
                if (name->Data() == "link4")
                {
                    link4Entity = entity;

                    std::cout
                        << "FOUND LINK4: "
                        << link4Entity
                        << std::endl;
                }
                if (name->Data() == "link1")
                {
                    link1Entity = entity;

                    std::cout
                        << "FOUND LINK1: "
                        << link1Entity
                        << std::endl;
                }
                if (name->Data() == "base_link")
                {
                    linkbEntity = entity;

                    std::cout
                        << "FOUND LINK1: "
                        << link1Entity
                        << std::endl;
                }
                if (name->Data() == "link2")
                {
                    link2Entity = entity;

                    std::cout
                        << "FOUND LINK2: "
                        << link2Entity
                        << std::endl;
                }

                if (name->Data() == "link5")
                {
                    link5Entity = entity;

                    std::cout
                        << "FOUND LINK5: "
                        << link5Entity
                        << std::endl;
                }
                if (name->Data() == "end_link")
                {
                    endLinkEntity = entity;

                    std::cout
                        << "FOUND END_LINK: "
                        << endLinkEntity
                        << std::endl;
                }

                return true;
            });

            std::cout << "==================================" << std::endl;
        }
        double k = 1000.0;
        if (link1Entity != gz::sim::kNullEntity &&
            link4Entity != gz::sim::kNullEntity &&
            endLinkEntity != gz::sim::kNullEntity &&
            linkbEntity != gz::sim::kNullEntity &&
            link5Entity != gz::sim::kNullEntity &&
            link2Entity != gz::sim::kNullEntity)
        {
            auto pose1Comp =
                ecm.Component<gz::sim::components::Pose>(
                    link1Entity);

            auto pose4Comp =
                ecm.Component<gz::sim::components::Pose>(
                    link4Entity);

            auto posebComp =
                ecm.Component<gz::sim::components::Pose>(
                    linkbEntity);

            auto pose2Comp =
                ecm.Component<gz::sim::components::Pose>(
                    link2Entity);

            auto pose5Comp =
                ecm.Component<gz::sim::components::Pose>(
                    link5Entity);

            auto poseEndComp =
                ecm.Component<gz::sim::components::Pose>(
                    endLinkEntity);

            if (pose1Comp && pose4Comp && posebComp)
            {
                auto pose1 = pose1Comp->Data();
                auto pose4 = pose4Comp->Data();
                auto poseb = posebComp->Data();

                // Attachment points in local link frames
                gz::math::Vector3d localA(0.0, 0.0, 0.25);
                gz::math::Vector3d localB(0.0, 0.0, 0.20);
                gz::math::Vector3d localC = poseb.Rot().Inverse().RotateVector(pose4.Pos() +
                pose4.Rot().RotateVector(localB)- poseb.Pos());

                


                // Convert to world coordinates
                auto worldA =
                    pose1.Pos() +
                    pose1.Rot().RotateVector(localA);

                auto worldB =
                    pose4.Pos() +
                    pose4.Rot().RotateVector(localB);

                auto error = worldA - worldB;

                double distance = error.Length();

                gz::math::Vector3d force = k * error;

                gz::sim::Link link4(link4Entity);
                gz::sim::Link linkb(linkbEntity);


                link4.AddWorldForce(
                    ecm,
                    force,
                    localB);
                linkb.AddWorldForce(
                    ecm,
                    -force,
                    localC);

            }
            if (pose2Comp && pose5Comp)
            {
                auto pose2 = pose2Comp->Data();
                auto pose5 = pose5Comp->Data();

                // Attachment points
                gz::math::Vector3d local2(0.0, 0.05, 0.30);
                gz::math::Vector3d local5(0.0, 0.0, 0.20);

                auto world2 =
                    pose2.Pos() +
                    pose2.Rot().RotateVector(local2);

                auto world5 =
                    pose5.Pos() +
                    pose5.Rot().RotateVector(local5);

                auto error2 = world2 - world5;

                double distance2 =
                    error2.Length();
                
                gz::math::Vector3d force2 =
                    k * error2;

                gz::sim::Link link2(link2Entity);
                gz::sim::Link link5(link5Entity);

                link5.AddWorldForce(
                    ecm,
                    force2,
                    local5);

                link2.AddWorldForce(
                    ecm,
                    -force2,
                    local2);
            }
            if (poseEndComp && posebComp)
            {
                auto poseEnd =
                    poseEndComp->Data();

                auto poseb =
                    posebComp->Data();

                double baseRoll =
                    poseb.Rot().Roll();

                double endRoll =
                    poseEnd.Rot().Roll();

                double desiredAngle =
                    -20.0 * M_PI / 180.0;

                double angleError =
                    (endRoll - baseRoll)
                    - desiredAngle;

                double kRot = 20.0;

                gz::math::Vector3d torque(
                    -kRot * angleError,
                    0.0,
                    0.0);

                gz::sim::Link endLink(
                    endLinkEntity);

                endLink.AddWorldWrench(
                    ecm,
                    gz::math::Vector3d::Zero,
                    torque);
            }
        }
        
    }
private:
    int counter = 0;
    bool printedLinks = false;
    gz::sim::Entity link4Entity =
        gz::sim::kNullEntity;
    gz::sim::Entity link1Entity =
        gz::sim::kNullEntity;
    gz::sim::Entity linkbEntity =
        gz::sim::kNullEntity;
    gz::sim::Entity link2Entity =
        gz::sim::kNullEntity;
    gz::sim::Entity link5Entity =
        gz::sim::kNullEntity;
    gz::sim::Entity endLinkEntity =
        gz::sim::kNullEntity;
};

}

GZ_ADD_PLUGIN(
    clg_plugins::SpringPlugin,
    gz::sim::System,
    clg_plugins::SpringPlugin::ISystemConfigure,
    clg_plugins::SpringPlugin::ISystemPreUpdate
)