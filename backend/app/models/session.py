from sqlmodel import Field, Relationship, SQLModel
from typing import List
from datetime import datetime

from typing import TYPE_CHECKING
if TYPE_CHECKING:
    from app.models.derived import CornerPerformance, Event
    from app.models.reference import Track, Car

class ACSession(SQLModel, table=True):
    __tablename__ = "sessions" # type: ignore

    session_id: int             = Field(primary_key=True)
    session_type: str | None    = Field(default=None, index=True)
    started_at: datetime | None = Field(default=None, index=True)

    track_id: int | None = Field(
        default=None,
        foreign_key="tracks.track_id"
    )

    car_id: int | None = Field(
        default=None,
        foreign_key="cars.car_id"
    )

    track: "Track"      = Relationship(back_populates="ac_sessions")
    car: "Car"          = Relationship(back_populates="ac_sessions")
    laps: List["Lap"]   = Relationship(back_populates="ac_session")


class Lap(SQLModel, table=True):
    __tablename__ = "laps" # type: ignore

    lap_id: int = Field(primary_key=True)
    lap_number: int | None  = Field(default=None)
    lap_time_ms: int | None = Field(default=None)
    is_valid: bool          = Field(default=True)

    session_id: int | None = Field(
        default=None,
        foreign_key="sessions.session_id"
    )

    ac_session: "ACSession"                         = Relationship(back_populates="laps")
    telemetry: List["Telemetry"]                    = Relationship(back_populates="lap")
    corner_performances: List["CornerPerformance"]  = Relationship(back_populates="lap")
    events: List["Event"]                           = Relationship(back_populates="lap")


class Telemetry(SQLModel, table=True):
    __tablename__ = "telemetry" # type: ignore

    telemetry_id: int               = Field(primary_key=True)
    packet_id: int | None           = Field(default=None)
    normalised_pos: float | None    = Field(default=None)
    speed_kmh: float | None         = Field(default=None)

    lap_id: int | None = Field(
        default=None,
        foreign_key="laps.lap_id"
    )

    lap: "Lap" = Relationship(back_populates="telemetry")
