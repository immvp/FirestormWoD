#include "instance_skyreach.h"

namespace MS
{
    class boss_Ranjit : public CreatureScript
    {
    public:
        // Entry: 86238
        boss_Ranjit()
            : CreatureScript("boss_Ranjit")
        {
        }

        enum class Spells : uint32
        {
            // Windwall.
            WINDWALL = 153315,      // 2:43:38 - 2-43-52. Random target. Every 14s.
            WINDWALL_AT_1 = 153311,
            WINDWALL_AT_2 = 153314,
            WINDWALL_MISSLE_1 = 153593,
            WINDWALL_MISSILE_2 = 153594,
            WINDWALL_DMG = 153759,
            // Four winds. Arrived after 2 or 3 instances of WindWall.
            FOUR_WINDS = 156793,
            FOUR_WINDS_DMG = 153139,
            FOUR_WINDS_AT_1 = 156634,
            FOUR_WINDS_AT_2 = 156636,
            FOUR_WINDS_VISUAL_1 = 166623,
            FOUR_WINDS_VISUAL_2 = 166664,
            // Fan of blades.
            FAN_OF_BLADES = 153757, // 2:43:34 - 2:43:50, every 16s.
            // Piercing rush.
            PIERCING_RUSH = 165731, // 2:43:29 - 2:43:44, every 15s.
        };

        enum class Texts : int32
        {
            COMBAT_START = -1603202,
            JUST_DIED = -1603203,
            FOUR_WINDS_1 = -1603204,
            FOUR_WINDS_2 = -1603205,
            KILL_PLAYER_1 = -1603206,
            KILL_PLAYER_2 = -1603207,
            VICTORY = -1603208
        };

        enum class Events : uint32
        {
            WINDWALL = 1,
            FOUR_WINDS = 2,
            PIERCING_RUSH = 3,
            FAN_OF_BLADES = 4,
            LENS_FLARE = 5
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new boss_RanjitAI(creature);
        }

        struct boss_RanjitAI : public ScriptedAI
        {
            boss_RanjitAI(Creature* creature) : ScriptedAI(creature),
            m_instance(creature->GetInstanceScript()),
            m_events(),
            m_countWindwalls(0)
            {
                m_TriggerFourWinds[0] = 0;
                m_TriggerFourWinds[1] = 0;
            }

            void Reset()
            {
                m_HasCombatStarted = false;
                m_events.Reset();

                m_countWindwalls = 0;

                if (!m_TriggerFourWinds[0])
                {
                    m_TriggerFourWinds[0] = me->SummonCreature(76119, 1165.871f, 1727.601f, 186)->GetGUID();
                }
                if (!m_TriggerFourWinds[1])
                {
                    m_TriggerFourWinds[1] = me->SummonCreature(76119, 1165.871f, 1727.601f, 189.4522f)->GetGUID();
                }
            }

            void JustDied(Unit* /*killer*/)
            {
                DoScriptText(int32(Texts::JUST_DIED), me);
            }

            void KilledUnit(Unit* /*victim*/)
            {
                if (urand(0, 1))
                    DoScriptText(int32(Texts::KILL_PLAYER_1), me);
                else
                    DoScriptText(int32(Texts::KILL_PLAYER_2), me);
            }

            void EnterCombat(Unit* who)
            {
                m_HasCombatStarted = true;
                m_events.ScheduleEvent(uint32(Events::WINDWALL), 8000);
                m_events.ScheduleEvent(uint32(Events::FAN_OF_BLADES), 5000);
                m_events.ScheduleEvent(uint32(Events::PIERCING_RUSH), 1000);

                DoScriptText(int32(Texts::COMBAT_START), me);
            }

            void UpdateAI(const uint32 diff)
            {
                if (!UpdateVictim())
                {
                    if (m_HasCombatStarted)
                    {
                        DoScriptText(int32(Texts::VICTORY), me);
                        m_HasCombatStarted = false;
                    }
                    return;
                }

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                m_events.Update(diff);

                while (uint32 eventId = m_events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                    case uint32(Events::FOUR_WINDS):
                        me->CastSpell(me->getVictim(), uint32(Spells::FOUR_WINDS));

                        m_countWindwalls = 0;
                        if (urand(0, 1))
                            DoScriptText(int32(Texts::FOUR_WINDS_1), me);
                        else
                            DoScriptText(int32(Texts::FOUR_WINDS_2), me);
                        break;
                    case uint32(Events::WINDWALL):
                        m_events.ScheduleEvent(uint32(Events::WINDWALL), urand(13000, 14000));

                        if (Unit* l_Unit = InstanceSkyreach::SelectRandomPlayerIncludedTank(me, 40.0f))
                            me->CastSpell(l_Unit, uint32(Spells::WINDWALL));

                        if (m_countWindwalls++ == 2)
                            m_events.ScheduleEvent(uint32(Events::FOUR_WINDS), urand(2000, 3000));
                        break;
                    case uint32(Events::FAN_OF_BLADES):
                        m_events.ScheduleEvent(uint32(Events::FAN_OF_BLADES), urand(15000, 17000));
                        me->CastSpell(me, uint32(Spells::FAN_OF_BLADES));
                        break;
                    case uint32(Events::PIERCING_RUSH):
                        m_events.ScheduleEvent(uint32(Events::PIERCING_RUSH), urand(13000, 16000));
                        if (Unit* l_Unit = InstanceSkyreach::SelectRandomPlayerExcludedTank(me, 40.0f))
                            me->CastSpell(l_Unit, uint32(Spells::WINDWALL));
                        break;
                    default:
                        break;
                    }

                    // If we cast something, we don't want to execute the other events.
                    if (me->HasUnitState(UNIT_STATE_CASTING))
                        return;
                }

                DoMeleeAttackIfReady();
            }

            InstanceScript* m_instance;
            EventMap m_events;
            uint32 m_countWindwalls;
            uint64 m_TriggerFourWinds[2];
            bool m_HasCombatStarted;
        };
    };
}

void AddSC_boss_ranjit()
{
    new MS::boss_Ranjit();
}